cheese_add_compile_flags()

function(cheese_build source)
    set(TEST_SOURCES ${source} ${ARGN})

    if (DEFINED CHEESE_TEST_NAME)
        set(TEST_TARGET ${CHEESE_TEST_NAME})
    else()
        get_filename_component(TEST_TARGET ${source} NAME_WE)
    endif()

    add_executable(${TEST_TARGET} ${TEST_SOURCES})
    target_link_libraries(${TEST_TARGET} PRIVATE common)
    if (CHEESE_TESTS_INSTALL)
        install(TARGETS ${TEST_TARGET} RUNTIME)
    endif()
endfunction()

function(cheese_test target)
    include(CMakeParseArguments)
    set(options)
    set(oneValueArgs NAME LABEL WORKING_DIRECTORY)
    set(multiValueArgs ARGS)
    cmake_parse_arguments(CHEESE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED CHEESE_TEST_LABEL)
        set(CHEESE_TEST_LABEL "main")
    endif()
    if (NOT DEFINED CHEESE_TEST_WORKING_DIRECTORY)
        set(CHEESE_TEST_WORKING_DIRECTORY .)
    endif()
    if (DEFINED CHEESE_TEST_NAME)
        set(TEST_NAME ${CHEESE_TEST_NAME})
    else()
        set(TEST_NAME ${target})
    endif()

    set(TEST_TARGET ${target})

    add_test(
        NAME ${TEST_NAME}
        WORKING_DIRECTORY ${CHEESE_TEST_WORKING_DIRECTORY}
        COMMAND $<TARGET_FILE:${TEST_TARGET}>
        ${CHEESE_TEST_ARGS})

    set_property(TEST ${TEST_NAME} PROPERTY LABELS ${CHEESE_TEST_LABEL})
endfunction()

function(cheese_test_cmd target)
    include(CMakeParseArguments)
    set(options)
    set(oneValueArgs NAME LABEL WORKING_DIRECTORY)
    set(multiValueArgs ARGS)
    cmake_parse_arguments(CHEESE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED CHEESE_TEST_LABEL)
        set(CHEESE_TEST_LABEL "main")
    endif()
    if (NOT DEFINED CHEESE_TEST_WORKING_DIRECTORY)
        set(CHEESE_TEST_WORKING_DIRECTORY .)
    endif()
    if (DEFINED CHEESE_TEST_NAME)
        set(TEST_NAME ${CHEESE_TEST_NAME})
    else()
        set(TEST_NAME ${target})
    endif()

    add_test(
        NAME ${TEST_NAME}
        WORKING_DIRECTORY ${CHEESE_TEST_WORKING_DIRECTORY}
        COMMAND ${target}
        ${CHEESE_TEST_ARGS})

    set_property(TEST ${TEST_NAME} PROPERTY LABELS ${CHEESE_TEST_LABEL})
endfunction()

# Builds and runs a test source file.
# Optional args:
# - NAME: name of the executable & test target (defaults to the source file name without extension)
# - LABEL: label for the test (defaults to main)
# - ARGS: arguments to pass to the test executable
# - WORKING_DIRECTORY
function(cheese_build_and_test source)
    include(CMakeParseArguments)
    set(options)
    set(oneValueArgs NAME LABEL WORKING_DIRECTORY)
    set(multiValueArgs ARGS)
    cmake_parse_arguments(CHEESE_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(TEST_SOURCES ${source} ${CHEESE_TEST_UNPARSED_ARGUMENTS} get-model.cpp)

    if (NOT DEFINED CHEESE_TEST_LABEL)
        set(CHEESE_TEST_LABEL "main")
    endif()
    if (NOT DEFINED CHEESE_TEST_WORKING_DIRECTORY)
        set(CHEESE_TEST_WORKING_DIRECTORY .)
    endif()
    if (DEFINED CHEESE_TEST_NAME)
        set(TEST_TARGET ${CHEESE_TEST_NAME})
    else()
        get_filename_component(TEST_TARGET ${source} NAME_WE)
    endif()

    add_executable(${TEST_TARGET} ${TEST_SOURCES})
    if (CHEESE_TESTS_INSTALL)
        install(TARGETS ${TEST_TARGET} RUNTIME)
    endif()
    target_link_libraries(${TEST_TARGET} PRIVATE common)

    add_test(
        NAME ${TEST_TARGET}
        WORKING_DIRECTORY ${CHEESE_TEST_WORKING_DIRECTORY}
        COMMAND $<TARGET_FILE:${TEST_TARGET}>
        ${CHEESE_TEST_ARGS})

    set_property(TEST ${TEST_TARGET} PROPERTY LABELS ${CHEESE_TEST_LABEL})
endfunction()

# build test-tokenizer-0 target once and add many tests
cheese_build(test-tokenizer-0.cpp)

cheese_test(test-tokenizer-0 NAME test-tokenizer-0-bert-bge          ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-bert-bge.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-command-r         ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-command-r.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-deepseek-coder    ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-deepseek-coder.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-deepseek-llm      ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-deepseek-llm.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-falcon            ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-falcon.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-gpt-2             ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-gpt-2.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-cheese-bpe         ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-cheese-bpe.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-cheese-spm         ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-cheese-spm.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-mpt               ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-mpt.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-phi-3             ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-phi-3.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-qwen2             ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-qwen2.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-refact            ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-refact.gguf)
cheese_test(test-tokenizer-0 NAME test-tokenizer-0-starcoder         ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-starcoder.gguf)

if (NOT WIN32)
    cheese_test_cmd(
        ${CMAKE_CURRENT_SOURCE_DIR}/test-tokenizers-repo.sh
        NAME test-tokenizers-ggml-vocabs
        WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        ARGS https://huggingface.co/ggml-org/vocabs ${PROJECT_SOURCE_DIR}/models/ggml-vocabs
    )
endif()

if (CHEESE_LLGUIDANCE)
    cheese_build_and_test(test-grammar-llguidance.cpp ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-cheese-bpe.gguf)
endif ()

if (NOT WIN32 OR NOT BUILD_SHARED_LIBS)
    # these tests are disabled on Windows because they use internal functions not exported with CHEESE_API (when building with shared libraries)
    cheese_build_and_test(test-sampling.cpp)
    cheese_build_and_test(test-grammar-parser.cpp)
    cheese_build_and_test(test-grammar-integration.cpp)
    cheese_build_and_test(test-cheese-grammar.cpp)
    cheese_build_and_test(test-chat.cpp WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    # TODO: disabled on loongarch64 because the ggml-ci node lacks Python 3.8
    if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "loongarch64")
        cheese_build_and_test(test-json-schema-to-grammar.cpp   WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
        target_include_directories(test-json-schema-to-grammar PRIVATE ${PROJECT_SOURCE_DIR}/tools/server)
    endif()

    if (NOT GGML_BACKEND_DL)
        cheese_build(test-quantize-stats.cpp)
    endif()

    cheese_build(test-gbnf-validator.cpp)

    # build test-tokenizer-1-bpe target once and add many tests
    cheese_build(test-tokenizer-1-bpe.cpp)

    # TODO: disabled due to slowness
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-aquila    ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-aquila.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-falcon    ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-falcon.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-gpt-2     ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-gpt-2.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-gpt-neox  ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-gpt-neox.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-cheese-bpe ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-cheese-bpe.gguf --ignore-merges)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-mpt       ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-mpt.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-refact    ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-refact.gguf)
    #cheese_test(test-tokenizer-1-bpe NAME test-tokenizer-1-starcoder ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-starcoder.gguf)

    # build test-tokenizer-1-spm target once and add many tests
    cheese_build(test-tokenizer-1-spm.cpp)

    cheese_test(test-tokenizer-1-spm  NAME test-tokenizer-1-cheese-spm ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-cheese-spm.gguf)
    #cheese_test(test-tokenizer-1-spm  NAME test-tokenizer-1-baichuan  ARGS ${PROJECT_SOURCE_DIR}/models/ggml-vocab-baichuan.gguf)

    # cheese_build_and_test(test-double-float.cpp) # SLOW
endif()

cheese_build_and_test(test-chat-parser.cpp)
cheese_build_and_test(test-chat-peg-parser.cpp peg-parser/simple-tokenize.cpp)
cheese_build_and_test(test-chat-template.cpp)
cheese_build_and_test(test-jinja.cpp)
cheese_test(test-jinja NAME test-jinja-py ARGS -py LABEL python)
cheese_build_and_test(test-json-partial.cpp)
cheese_build_and_test(test-log.cpp)
cheese_build_and_test(
    test-peg-parser.cpp
    peg-parser/simple-tokenize.cpp
    peg-parser/test-basic.cpp
    peg-parser/test-gbnf-generation.cpp
    peg-parser/test-json-parser.cpp
    peg-parser/test-json-serialization.cpp
    peg-parser/test-unicode.cpp
    peg-parser/tests.h
)
cheese_build_and_test(test-regex-partial.cpp)

if (NOT ${CMAKE_SYSTEM_PROCESSOR} MATCHES "s390x")
    set(MODEL_NAME "tinyllamas/stories15M-q4_0.gguf")
    set(MODEL_HASH "SHA256=66967fbece6dbe97886593fdbb73589584927e29119ec31f08090732d1861739")
else()
    set(MODEL_NAME "tinyllamas/stories15M-be.Q4_0.gguf")
    set(MODEL_HASH "SHA256=9aec857937849d976f30397e97eb1cabb53eb9dcb1ce4611ba8247fb5f44c65d")
endif()
set(MODEL_DEST "${CMAKE_BINARY_DIR}/${MODEL_NAME}")

add_test(NAME test-download-model COMMAND ${CMAKE_COMMAND}
    -DDEST=${MODEL_DEST}
    -DNAME=${MODEL_NAME}
    -DHASH=${MODEL_HASH}
    -P ${CMAKE_SOURCE_DIR}/cmake/download-models.cmake
)
set_tests_properties(test-download-model PROPERTIES FIXTURES_SETUP test-download-model)

cheese_build_and_test(test-thread-safety.cpp ARGS -m "${MODEL_DEST}" -ngl 99 -p "The meaning of life is" -n 128 -c 256 -ub 32 -np 4 -t 2)
set_tests_properties(test-thread-safety PROPERTIES FIXTURES_REQUIRED test-download-model)

cheese_build_and_test(test-arg-parser.cpp)

if (NOT CHEESE_SANITIZE_ADDRESS AND NOT GGML_SCHED_NO_REALLOC)
  # TODO: repair known memory leaks
  cheese_build_and_test(test-opt.cpp)
endif()
cheese_build_and_test(test-gguf.cpp)
cheese_build_and_test(test-backend-ops.cpp)

cheese_build_and_test(test-model-load-cancel.cpp LABEL "model")
cheese_build_and_test(test-autorelease.cpp       LABEL "model")
cheese_build_and_test(test-backend-sampler.cpp   LABEL "model")

# Test for state restore with fragmented KV cache
# Requires a model, uses same args pattern as test-thread-safety
cheese_build_and_test(test-state-restore-fragmented.cpp LABEL "model" ARGS -m "${MODEL_DEST}")
set_tests_properties(test-state-restore-fragmented PROPERTIES FIXTURES_REQUIRED test-download-model)

if (NOT GGML_BACKEND_DL)
    # these tests use the backends directly and cannot be built with dynamic loading
    cheese_build_and_test(test-barrier.cpp)
    cheese_build_and_test(test-quantize-fns.cpp)
    cheese_build_and_test(test-quantize-perf.cpp)
    cheese_build_and_test(test-rope.cpp)
endif()

# libmtmd
set(CHEESE_TEST_NAME test-mtmd-c-api)
cheese_build_and_test(test-mtmd-c-api.c)
target_link_libraries(${CHEESE_TEST_NAME} PRIVATE mtmd)

# GGUF model data fetcher library for tests that need real model metadata
# Only compile when cpp-httplib has SSL support (CPPHTTPLIB_OPENSSL_SUPPORT)
if (TARGET cpp-httplib)
    get_target_property(_cpp_httplib_defs cpp-httplib INTERFACE_COMPILE_DEFINITIONS)
    if (_cpp_httplib_defs MATCHES "CPPHTTPLIB_OPENSSL_SUPPORT")
        add_library(gguf-model-data STATIC gguf-model-data.cpp)
        target_link_libraries(gguf-model-data PRIVATE common cpp-httplib)
        target_include_directories(gguf-model-data PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

        add_executable(test-gguf-model-data test-gguf-model-data.cpp)
        target_link_libraries(test-gguf-model-data PRIVATE gguf-model-data common)
        cheese_test(test-gguf-model-data LABEL "model")
    endif()
endif()

# dummy executable - not installed
get_filename_component(TEST_TARGET test-c.c NAME_WE)
add_executable(${TEST_TARGET} test-c.c)
target_link_libraries(${TEST_TARGET} PRIVATE cheese)

cheese_build_and_test(test-alloc.cpp)
target_include_directories(test-alloc PRIVATE ${PROJECT_SOURCE_DIR}/ggml/src)

cheese_build_and_test(test-edge.cpp)
target_include_directories(test-edge PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(test-edge PRIVATE ${PROJECT_SOURCE_DIR}/third_party/palloc/include)
target_include_directories(test-edge PRIVATE ${PROJECT_SOURCE_DIR}/third_party/contextsqueezer/native/include)
target_link_libraries(test-edge PRIVATE palloc contextsqueeze)
