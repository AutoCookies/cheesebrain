// Unit tests for PromptBuilder (3-combo: pomaicache, contextsqueezer, palloc).
// Requires a GGUF model with vocab when run; skips gracefully if no model.
//
#ifdef NDEBUG
#undef NDEBUG
#endif

#include "cheesebrain/prompt_builder.h"
#include "chat.h"
#include "common.h"
#include "get-model.h"
#include "pomaicache.h"
#include "palloc/arena_pomai.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>

struct model_and_ctx {
    cheese_model * model = nullptr;
    cheese_context * ctx = nullptr;
    ~model_and_ctx() {
        if (ctx) { cheese_free(ctx); }
        if (model) { cheese_model_free(model); }
    }
};

static bool load_model_and_ctx(const char * model_path, model_and_ctx & out) {
    out.model = cheese_model_load_from_file(model_path, cheese_model_default_params());
    if (!out.model) {
        return false;
    }
    cheese_context_params cparams = cheese_context_default_params();
    cparams.n_ctx   = 256;
    cparams.n_batch = 256;
    out.ctx = cheese_init_from_model(out.model, cparams);
    if (!out.ctx) {
        cheese_model_free(out.model);
        out.model = nullptr;
        return false;
    }
    return true;
}

int main(int argc, char ** argv) {
    char * model_path = get_model_or_exit(argc, argv);
    if (!model_path || std::strlen(model_path) == 0) {
        return 0; // skip when no model
    }

    model_and_ctx mc;
    if (!load_model_and_ctx(model_path, mc)) {
        std::fprintf(stderr, "test-prompt-builder: failed to load model, skipping\n");
        return 0;
    }
    cheese_context * ctx = mc.ctx;

    prompt_cache_config cfg;
    cfg.prompt_cache_enabled           = true;
    cfg.prompt_cache_prefix_min_tokens = 32; // low for test
    cfg.prompt_cache_ttl_ms            = 60000;
    cfg.contextsqueeze_min_chars       = 100;
    cfg.contextsqueeze_aggressiveness  = 1;
    cfg.tokenizer_id                   = "test";

    const size_t arena_size = 1024 * 1024;
    pa_arena_t * arena = p_arena_create(arena_size);
    assert(arena != nullptr);

    pomaicache::Config pomai_cfg;
    pomai_cfg.memory_limit_bytes = 4 * 1024 * 1024;
    pomai_cfg.data_dir           = "./test_pomai_data";
    std::unique_ptr<pomaicache::PomaiCache> cache;
    try {
        cache = std::make_unique<pomaicache::PomaiCache>(pomai_cfg);
    } catch (const std::exception & e) {
        std::fprintf(stderr, "test-prompt-builder: pomaicache init failed: %s, skipping\n", e.what());
        p_arena_destroy(arena);
        return 0;
    }

    common_chat_params chat_params;
    chat_params.prompt = "Hello, world. This is a short prompt for testing.";

    prompt_builder pb(ctx, cache.get(), cfg, arena);
    prompt_metrics metrics;
    prompt_build_result result = pb.build_and_maybe_cache(chat_params, 0, metrics);

    assert(!result.tokens.empty());
    assert(metrics.compression_ratio >= 0.0 && metrics.compression_ratio <= 1.01);
    assert(result.cached_prefix_tokens >= 0);
    assert(result.suffix_tokens >= 0);

    if (result.tokens.size() >= static_cast<size_t>(cfg.prompt_cache_prefix_min_tokens)) {
        pb.save_prefix_after_eval(0, result.tokens, metrics);
        assert(metrics.cache_creation_tokens == result.tokens.size() || metrics.cache_creation_tokens == 0);
    }

    p_arena_reset(arena);
    prompt_metrics metrics2;
    prompt_build_result result2 = pb.build_and_maybe_cache(chat_params, 0, metrics2);
    assert(!result2.tokens.empty());
    assert(result2.tokens.size() == result.tokens.size());
    for (size_t i = 0; i < result.tokens.size(); ++i) {
        assert(result2.tokens[i] == result.tokens[i]);
    }

    p_arena_destroy(arena);
    std::printf("test-prompt-builder: passed\n");
    return 0;
}
