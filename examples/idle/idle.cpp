#include "arg.h"
#include "common.h"
#include "log.h"
#include "cheese.h"

#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

static void print_usage(int /*argc*/, char ** argv) {
    printf("\nexample usage:\n");
    printf("\n    %s -m model.gguf [-ngl n_gpu_layers]\n", argv[0]);
    printf("\n");
}

int main(int argc, char ** argv) {
    common_params params;

    if (!common_params_parse(argc, argv, params, CHEESE_EXAMPLE_COMMON, print_usage)) {
        return 1;
    }

    common_init();

    // init LLM

    cheese_backend_init();
    cheese_numa_init(params.numa);

    // initialize the model

    cheese_model_params model_params = common_model_params_to_cheese(params);

    cheese_model * model = cheese_model_load_from_file(params.model.path.c_str(), model_params);

    if (model == NULL) {
        LOG_ERR("%s: error: unable to load model\n" , __func__);
        return 1;
    }

    const cheese_vocab * vocab = cheese_model_get_vocab(model);

    // we need just a dummy token to evaluate
    std::vector<cheese_token> prompt_tokens(1, cheese_vocab_bos(vocab));

    cheese_context_params ctx_params = cheese_context_default_params();
    ctx_params.n_ctx   = 512;
    ctx_params.n_batch = 512;
    ctx_params.no_perf = false;

    cheese_context * ctx = cheese_init_from_model(model, ctx_params);
    if (ctx == NULL) {
        fprintf(stderr , "%s: error: failed to create the cheese_context\n" , __func__);
        return 1;
    }

    cheese_batch batch = cheese_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    const int n_iters = 3;

    // warm-up
    cheese_decode(ctx, batch);
    cheese_memory_clear(cheese_get_memory(ctx), true);
    cheese_synchronize(ctx);

    for (int64_t t_pause_ms = 0; t_pause_ms <= 4000; t_pause_ms += 800) {
        double t_sum_us  = 0.0;
        double t_sum2_us = 0.0;

        for (int i = 0; i < n_iters; i++) {
            // this pause is important - it simulates "idle GPU"
            std::this_thread::sleep_for(std::chrono::milliseconds(t_pause_ms));

            const int64_t t_start_us = cheese_time_us();

            // this should take constant time
            cheese_decode(ctx, batch);
            cheese_synchronize(ctx);

            const int64_t t_end_us = cheese_time_us();

            const double t_cur_us = t_end_us - t_start_us;

#if 1
            // print individual decode times
            printf("  - decode time: %8.2f ms\n", t_cur_us / 1000);
#endif

            t_sum_us  += t_cur_us;
            t_sum2_us += t_cur_us * t_cur_us;

            cheese_memory_clear(cheese_get_memory(ctx), true);
            cheese_synchronize(ctx); // just in case
        }

        const double t_avg_us = t_sum_us / n_iters;
        const double t_dev_us = sqrt((t_sum2_us / (n_iters - 1)) - (t_avg_us * t_avg_us * n_iters) / (n_iters - 1));

        printf("iters: %4d, pause: %5d ms, avg decode time: %8.2f +/- %4.2f ms\n", n_iters, (int) t_pause_ms, t_avg_us / 1000, t_dev_us / 1000);
        fflush(stdout);
    }

    cheese_free(ctx);
    cheese_model_free(model);

    return 0;
}
