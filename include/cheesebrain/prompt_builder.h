// Pomai Cheesebrain PromptBuilder
// Central orchestration for context compression, bounded allocations and
// prompt-prefix caching. Implementation lives in src/prompt_builder.cpp.
//
// Prompt caching is Anthropic-style: cache only stable prefixes (e.g. system
// prompt, tool definitions), min length 1024 tokens, TTL and breakpoint limits.

#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "cheese.h"

using cheese_tokens = std::vector<cheese_token>;

struct common_chat_params;

namespace pomaicache {
class PomaiCache;
}

// Configuration for prompt caching and context squeezing.
// Values are usually populated from common_params.
struct prompt_cache_config {
    bool    prompt_cache_enabled           = true;
    int32_t prompt_cache_prefix_min_tokens = 1024;
    int64_t prompt_cache_ttl_ms            = 300000;
    int32_t prompt_cache_max_breakpoints   = 4;
    int32_t prompt_cache_lookback_blocks   = 20;
    int32_t contextsqueeze_aggressiveness  = 6;
    int32_t contextsqueeze_min_chars       = 4096;
    std::string tokenizer_id               = "cheese";  // model/tokenizer id for cache keys
};

// Per-request metrics emitted by PromptBuilder.
struct prompt_metrics {
    uint64_t cache_read_tokens      = 0;
    uint64_t cache_creation_tokens  = 0;
    double   cache_savings_ratio    = 0.0;
    double   compression_ratio      = 1.0;
    size_t   arena_bytes_used       = 0;
};

// Result of building a prompt for inference.
struct prompt_build_result {
    cheese_tokens tokens;

    int32_t cached_prefix_tokens = 0;
    int32_t suffix_tokens        = 0;
    bool    cache_hit            = false;
};

// PromptBuilder ties together:
//  - chat templates / prompt formatting
//  - optional context squeezing
//  - prompt prefix caching via pomaicache
class prompt_builder {
public:
    prompt_builder(
            cheese_context * ctx,
            pomaicache::PomaiCache * cache,
            const prompt_cache_config & cfg);

    // Build tokens for the given chat parameters and collect basic metrics.
    // If prompt cache is enabled and a cached prefix matches, restores KV state
    // for slot_id and returns cache_hit + cached_prefix_tokens/suffix_tokens.
    // Otherwise tokenizes (after optional squeeze), returns tokens for inference.
    prompt_build_result build_and_maybe_cache(
            const common_chat_params & chat_params,
            cheese_seq_id slot_id,
            prompt_metrics & metrics);

    // After inference has run for the prompt, call this to store the prefix KV
    // in pomaicache for future reuse. Uses the same prefix policy (min length,
    // single block when no message boundaries). No-op if cache disabled or
    // prefix too short.
    void save_prefix_after_eval(
            cheese_seq_id slot_id,
            const cheese_tokens & tokens,
            prompt_metrics & metrics);

private:
    cheese_context *         ctx;
    pomaicache::PomaiCache * cache;
    prompt_cache_config      cfg;
};

