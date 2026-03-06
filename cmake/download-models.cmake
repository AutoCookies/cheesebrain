get_filename_component(DEST_DIR "${DEST}" DIRECTORY)
file(MAKE_DIRECTORY "${DEST_DIR}")

if(NOT EXISTS "${DEST}")
    message(STATUS "Downloading ${NAME} from ggml-org/models...")
    # Do not use EXPECTED_HASH: when download fails CMake errors with "cannot calculate hash if file is not saved"
    file(DOWNLOAD
        "https://huggingface.co/ggml-org/models/resolve/main/${NAME}?download=true"
        "${DEST}"
        TLS_VERIFY ON
        STATUS status
    )
    list(GET status 0 code)
    if(NOT code EQUAL 0)
        list(GET status 1 msg)
        message(STATUS "Download skipped or failed (${msg}); model-dependent tests will skip.")
        if(EXISTS "${DEST}")
            file(REMOVE "${DEST}")
        endif()
    endif()
endif()
