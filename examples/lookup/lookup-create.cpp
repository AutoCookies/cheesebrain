#include "arg.h"
#include "common.h"
#include "ngram-cache.h"
#include "cheese.h"

#include <string>
#include <vector>

int main(int argc, char ** argv){
    common_params params;

    if (!common_params_parse(argc, argv, params, CHEESE_EXAMPLE_LOOKUP)) {
        return 1;
    }

    // init cheese.cpp
    cheese_backend_init();
    cheese_numa_init(params.numa);

    // load the model
    auto cheese_init = common_init_from_params(params);

    auto * model = cheese_init->model();
    auto * ctx = cheese_init->context();

    GGML_ASSERT(model != nullptr);

    // tokenize the prompt
    std::vector<cheese_token> inp;
    inp = common_tokenize(ctx, params.prompt, true, true);
    fprintf(stderr, "%s: tokenization done\n", __func__);

    common_ngram_cache ngram_cache;
    common_ngram_cache_update(ngram_cache, CHEESE_NGRAM_STATIC, CHEESE_NGRAM_STATIC, inp, inp.size(), true);
    fprintf(stderr, "%s: hashing done, writing file to %s\n", __func__, params.speculative.lookup_cache_static.c_str());

    common_ngram_cache_save(ngram_cache, params.speculative.lookup_cache_static);

    return 0;
}
