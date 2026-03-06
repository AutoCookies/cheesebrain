# Run test-eval-callback only if MODEL_DEST exists and has valid size (download succeeded).
# When download is skipped, the file is missing or empty; skip the test in that case.
if(EXISTS "${MODEL_DEST}")
    file(SIZE "${MODEL_DEST}" MODEL_SIZE)
    if(MODEL_SIZE GREATER 100000)
        execute_process(
            COMMAND "${TEST_EXEC}" -m "${MODEL_DEST}" --prompt hello --seed 42 -ngl 0
            RESULT_VARIABLE r
        )
        if(NOT r EQUAL 0)
            message(FATAL_ERROR "test-eval-callback failed with exit code: ${r}")
        endif()
    else()
        message(STATUS "Skipping test-eval-callback: model file too small or invalid")
    endif()
else()
    message(STATUS "Skipping test-eval-callback: model file not found")
endif()
