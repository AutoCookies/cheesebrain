#include "arg.h"
#include "common.h"
#include "debug.h"
#include "log.h"
#include "cheese.h"
#include "cheese-cpp.h"
#include <string>
#include <vector>

static bool run(cheese_context * ctx, const common_params & params) {
    const cheese_model * model = cheese_get_model(ctx);
    const cheese_vocab * vocab = cheese_model_get_vocab(model);

    const bool add_bos = cheese_vocab_get_add_bos(vocab);

    std::vector<cheese_token> tokens = common_tokenize(ctx, params.prompt, add_bos);

    if (tokens.empty()) {
        LOG_ERR("%s : there are not input tokens to process - (try to provide a prompt with '-p')\n", __func__);
        return false;
    }

    if (cheese_decode(ctx, cheese_batch_get_one(tokens.data(), tokens.size()))) {
        LOG_ERR("%s : failed to eval\n", __func__);
        return false;
    }

    return true;
}

int main(int argc, char ** argv) {
    base_callback_data cb_data;

    common_params params;

    if (!common_params_parse(argc, argv, params, CHEESE_EXAMPLE_COMMON)) {
        return 1;
    }

    common_init();

    cheese_backend_init();
    cheese_numa_init(params.numa);

    // pass the callback to the backend scheduler
    // it will be executed for each node during the graph computation
    params.cb_eval = common_debug_cb_eval<false>;
    params.cb_eval_user_data = &cb_data;
    params.warmup = false;

    // init
    auto cheese_init = common_init_from_params(params);

    auto * model = cheese_init->model();
    auto * ctx   = cheese_init->context();

    if (model == nullptr || ctx == nullptr) {
        LOG_ERR("%s : failed to init\n", __func__);
        return 1;
    }

    // print system information
    {
        LOG_INF("\n");
        LOG_INF("%s\n", common_params_get_system_info(params).c_str());
        LOG_INF("\n");
    }

    bool OK = run(ctx, params);
    if (!OK) {
        return 1;
    }

    LOG("\n");
    cheese_perf_context_print(ctx);

    cheese_backend_free();

    return 0;
}
