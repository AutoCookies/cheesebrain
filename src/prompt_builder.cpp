#include "cheesebrain/prompt_builder.h"

#include "chat.h"
#include "common.h"
#include "palloc_cxx_arena.h"

#include <algorithm>
#include <cstring>
#include <sstream>

extern "C" {
#include "contextsqueeze.h"
}

namespace {

// FNV-1a 64-bit hash of data; result as hex string for cache keys.
std::string fnv1a_hex(const uint8_t * data, size_t len) {
    constexpr uint64_t fnv_prime = 0x100000001b3ULL;
    constexpr uint64_t fnv_basis = 0xcbf29ce484222325ULL;
    uint64_t h = fnv_basis;
    for (size_t i = 0; i < len; ++i) {
        h ^= static_cast<uint64_t>(data[i]);
        h *= fnv_prime;
    }
    std::ostringstream os;
    os << std::hex << h;
    return os.str();
}

// Serialize prefix of tokens to bytes (4 bytes per token, little-endian) for hashing.
std::vector<uint8_t> serialize_tokens_for_hash(const cheese_tokens & tokens, size_t prefix_len) {
    const size_t n = std::min(prefix_len, tokens.size());
    std::vector<uint8_t> out(n * sizeof(cheese_token));
    const cheese_token * src = tokens.data();
    uint8_t * dst = out.data();
    for (size_t i = 0; i < n; ++i) {
        uint32_t v = static_cast<uint32_t>(src[i]);
        dst[i * 4 + 0] = static_cast<uint8_t>(v);
        dst[i * 4 + 1] = static_cast<uint8_t>(v >> 8);
        dst[i * 4 + 2] = static_cast<uint8_t>(v >> 16);
        dst[i * 4 + 3] = static_cast<uint8_t>(v >> 24);
    }
    return out;
}

// Build pomaicache key for prompt prefix: prm:<tokenizer_id>:<hash_hex>
std::string prompt_cache_key(const std::string & tokenizer_id, const std::string & hash_hex) {
    return "prm:" + tokenizer_id + ":" + hash_hex;
}

} // namespace

prompt_builder::prompt_builder(
        cheese_context * ctx,
        pomaicache::PomaiCache * cache,
        const prompt_cache_config & cfg,
        void * arena_handle)
    : ctx(ctx)
    , cache(cache)
    , cfg(cfg)
    , arena_handle(arena_handle) {
}

// Compress a single prompt string if it is large enough; otherwise return unchanged.
static std::string squeeze_prompt_if_needed(
        const std::string & prompt,
        int32_t min_chars,
        int32_t aggressiveness,
        double & out_ratio) {
    if (static_cast<int32_t>(prompt.size()) < min_chars || aggressiveness <= 0) {
        out_ratio = 1.0;
        return prompt;
    }

    csq_view in;
    in.data = prompt.c_str();
    in.len  = prompt.size();

    csq_buf out;
    out.data = nullptr;
    out.len  = 0;

    const int rc = csq_squeeze_ex(in, aggressiveness, &out);
    if (rc != CSQ_OK || out.data == nullptr || out.len == 0) {
        out_ratio = 1.0;
        return prompt;
    }

    std::string squeezed(out.data, out.len);
    csq_free(&out);

    const double original   = static_cast<double>(prompt.size());
    const double compressed = static_cast<double>(squeezed.size());
    out_ratio = original > 0.0 ? (compressed / original) : 1.0;

    return squeezed;
}

prompt_build_result prompt_builder::build_and_maybe_cache(
        const common_chat_params & chat_params,
        cheese_seq_id slot_id,
        prompt_metrics & metrics) {
    prompt_build_result result;

    palloc::PomaiArenaAllocator<char> char_alloc(arena_handle);

    double compression_ratio = 1.0;
    const std::string squeezed = squeeze_prompt_if_needed(
            chat_params.prompt,
            cfg.contextsqueeze_min_chars,
            cfg.contextsqueeze_aggressiveness,
            compression_ratio);
    metrics.compression_ratio = compression_ratio;

    // Tokenize with BOS/special tokens to match server behaviour.
    cheese_tokens tokens = common_tokenize(ctx, squeezed, true, true);
    if (tokens.empty()) {
        result.tokens = std::move(tokens);
        result.cached_prefix_tokens = 0;
        result.suffix_tokens        = 0;
        result.cache_hit            = false;
        return result;
    }

    const size_t total_tokens = tokens.size();
    result.tokens = tokens;

    // Anthropic-style policy: cache only if enabled and prefix is long enough.
    // Single-block mode (no message boundaries here): entire prompt is the prefix.
    const bool cacheable = cfg.prompt_cache_enabled && cache != nullptr
            && total_tokens >= static_cast<size_t>(cfg.prompt_cache_prefix_min_tokens);

    if (!cacheable) {
        result.cached_prefix_tokens = 0;
        result.suffix_tokens         = static_cast<int32_t>(total_tokens);
        result.cache_hit             = false;
        return result;
    }

    const size_t prefix_len = total_tokens;
    std::vector<uint8_t> prefix_bytes = serialize_tokens_for_hash(tokens, prefix_len);
    const std::string hash_hex = fnv1a_hex(prefix_bytes.data(), prefix_bytes.size());
    const std::string key = prompt_cache_key(cfg.tokenizer_id, hash_hex);

    auto cached = cache->Get(key);
    if (cached.has_value() && !cached->empty()) {
        const size_t kv_size = cached->size();
        const size_t written = cheese_state_seq_set_data_ext(
                ctx, reinterpret_cast<const uint8_t *>(cached->data()), kv_size, slot_id, 0);
        if (written == kv_size) {
            result.cache_hit            = true;
            result.cached_prefix_tokens  = static_cast<int32_t>(prefix_len);
            result.suffix_tokens         = 0;
            metrics.cache_read_tokens    = prefix_len;
            const double denom = static_cast<double>(prefix_len);
            metrics.cache_savings_ratio  = denom > 0.0 ? 1.0 : 0.0;
            return result;
        }
    }

    result.cached_prefix_tokens = 0;
    result.suffix_tokens         = static_cast<int32_t>(total_tokens);
    result.cache_hit            = false;
    return result;
}

void prompt_builder::save_prefix_after_eval(
        cheese_seq_id slot_id,
        const cheese_tokens & tokens,
        prompt_metrics & metrics) {
    if (!cfg.prompt_cache_enabled || cache == nullptr || tokens.empty()) {
        return;
    }
    const size_t total_tokens = tokens.size();
    if (total_tokens < static_cast<size_t>(cfg.prompt_cache_prefix_min_tokens)) {
        return;
    }

    const size_t prefix_len = total_tokens;
    std::vector<uint8_t> prefix_bytes = serialize_tokens_for_hash(tokens, prefix_len);
    const std::string hash_hex = fnv1a_hex(prefix_bytes.data(), prefix_bytes.size());
    const std::string key = prompt_cache_key(cfg.tokenizer_id, hash_hex);

    const size_t kv_size = cheese_state_seq_get_size_ext(ctx, slot_id, 0);
    if (kv_size == 0) {
        return;
    }

    std::vector<std::byte> kv_buf(kv_size);
    const size_t nread = cheese_state_seq_get_data_ext(
            ctx, reinterpret_cast<uint8_t *>(kv_buf.data()), kv_size, slot_id, 0);
    if (nread != kv_size) {
        return;
    }

    pomaicache::Ttl ttl;
    ttl.ms = static_cast<uint64_t>(cfg.prompt_cache_ttl_ms);
    const bool ok = cache->Set(key, std::span<const std::byte>(kv_buf.data(), kv_buf.size()), ttl);
    if (ok) {
        metrics.cache_creation_tokens = prefix_len;
    }
}
