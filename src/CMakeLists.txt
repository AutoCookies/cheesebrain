cheese_add_compile_flags()

#
# libraries
#

# cheese

add_library(cheese
            ../include/cheese.h
            cheese.cpp
            cheese-adapter.cpp
            cheese-arch.cpp
            cheese-batch.cpp
            cheese-chat.cpp
            cheese-context.cpp
            cheese-cparams.cpp
            cheese-grammar.cpp
            cheese-graph.cpp
            cheese-hparams.cpp
            cheese-impl.cpp
            cheese-io.cpp
            cheese-kv-cache.cpp
            cheese-kv-cache-iswa.cpp
            cheese-memory.cpp
            cheese-memory-hybrid.cpp
            cheese-memory-hybrid-iswa.cpp
            cheese-memory-recurrent.cpp
            cheese-mmap.cpp
            cheese-model-loader.cpp
            cheese-model-saver.cpp
            cheese-model.cpp
            cheese-model-api.cpp
            cheese-quant.cpp
            cheese-sampler.cpp
            cheese-vocab.cpp
            unicode-data.cpp
            unicode.cpp
            unicode.h
            models/afmoe.cpp
            models/apertus.cpp
            models/arcee.cpp
            models/arctic.cpp
            models/arwkv7.cpp
            models/baichuan.cpp
            models/bailingmoe.cpp
            models/bailingmoe2.cpp
            models/bert.cpp
            models/bitnet.cpp
            models/bloom.cpp
            models/chameleon.cpp
            models/chatglm.cpp
            models/codeshell.cpp
            models/cogvlm.cpp
            models/cohere2-iswa.cpp
            models/command-r.cpp
            models/dbrx.cpp
            models/deci.cpp
            models/deepseek.cpp
            models/deepseek2.cpp
            models/delta-net-base.cpp
            models/dots1.cpp
            models/dream.cpp
            models/ernie4-5-moe.cpp
            models/ernie4-5.cpp
            models/eurobert.cpp
            models/exaone-moe.cpp
            models/exaone.cpp
            models/exaone4.cpp
            models/falcon-h1.cpp
            models/falcon.cpp
            models/gemma-embedding.cpp
            models/gemma.cpp
            models/gemma2-iswa.cpp
            models/gemma3.cpp
            models/gemma3n-iswa.cpp
            models/glm4-moe.cpp
            models/glm4.cpp
            models/gpt2.cpp
            models/gptneox.cpp
            models/granite-hybrid.cpp
            models/granite.cpp
            models/grok.cpp
            models/grovemoe.cpp
            models/hunyuan-dense.cpp
            models/hunyuan-moe.cpp
            models/internlm2.cpp
            models/jais.cpp
            models/jais2.cpp
            models/jamba.cpp
            models/kimi-linear.cpp
            models/lfm2.cpp
            models/llada-moe.cpp
            models/llada.cpp
            models/cheese-iswa.cpp
            models/cheese.cpp
            models/maincoder.cpp
            models/mamba-base.cpp
            models/mamba.cpp
            models/mimo2-iswa.cpp
            models/minicpm3.cpp
            models/minimax-m2.cpp
            models/mistral3.cpp
            models/modern-bert.cpp
            models/mpt.cpp
            models/nemotron-h.cpp
            models/nemotron.cpp
            models/neo-bert.cpp
            models/olmo.cpp
            models/olmo2.cpp
            models/olmoe.cpp
            models/openai-moe-iswa.cpp
            models/openelm.cpp
            models/orion.cpp
            models/paddleocr.cpp
            models/pangu-embedded.cpp
            models/phi2.cpp
            models/phi3.cpp
            models/plamo.cpp
            models/plamo2.cpp
            models/plamo3.cpp
            models/plm.cpp
            models/qwen.cpp
            models/qwen2.cpp
            models/qwen2moe.cpp
            models/qwen2vl.cpp
            models/qwen3.cpp
            models/qwen35.cpp
            models/qwen35moe.cpp
            models/qwen3moe.cpp
            models/qwen3next.cpp
            models/qwen3vl-moe.cpp
            models/qwen3vl.cpp
            models/refact.cpp
            models/rnd1.cpp
            models/rwkv6-base.cpp
            models/rwkv6.cpp
            models/rwkv6qwen2.cpp
            models/rwkv7-base.cpp
            models/rwkv7.cpp
            models/seed-oss.cpp
            models/smallthinker.cpp
            models/smollm3.cpp
            models/stablelm.cpp
            models/starcoder.cpp
            models/starcoder2.cpp
            models/step35-iswa.cpp
            models/t5-dec.cpp
            models/t5-enc.cpp
            models/wavtokenizer-dec.cpp
            models/xverse.cpp
            )

set_target_properties(cheese PROPERTIES
    VERSION ${CHEESE_INSTALL_VERSION}
    SOVERSION 0
    MACHO_CURRENT_VERSION 0 # keep macOS linker from seeing oversized version number
)

target_include_directories(cheese PRIVATE .)
target_include_directories(cheese PUBLIC ../include)
target_compile_features   (cheese PRIVATE cxx_std_17) # don't bump

target_link_libraries(cheese PUBLIC ggml)

if (BUILD_SHARED_LIBS)
    set_target_properties(cheese PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_compile_definitions(cheese PRIVATE CHEESE_BUILD)
    target_compile_definitions(cheese PUBLIC  CHEESE_SHARED)
endif()
