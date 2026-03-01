// ref: https://github.com/ggml-org/cheese.cpp/issues/4952#issuecomment-1892864763

#include <cstdio>
#include <string>
#include <thread>

#include "cheese.h"
#include "get-model.h"

// This creates a new context inside a pthread and then tries to exit cleanly.
int main(int argc, char ** argv) {
    auto * model_path = get_model_or_exit(argc, argv);

    std::thread([&model_path]() {
        cheese_backend_init();
        auto * model = cheese_model_load_from_file(model_path, cheese_model_default_params());
        auto * ctx = cheese_init_from_model(model, cheese_context_default_params());
        cheese_free(ctx);
        cheese_model_free(model);
        cheese_backend_free();
    }).join();

    return 0;
}
