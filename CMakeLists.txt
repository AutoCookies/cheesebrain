cmake_minimum_required(VERSION 3.14...3.28) # for add_link_options and implicit target directories.
project("cheese.cpp" C CXX)
include(CheckIncludeFileCXX)

#set(CMAKE_WARN_DEPRECATED YES)
set(CMAKE_WARN_UNUSED_CLI YES)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT XCODE AND NOT MSVC AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

# Add path to modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CHEESE_STANDALONE ON)

    include(git-vars)

    # configure project version
    # TODO
else()
    set(CHEESE_STANDALONE OFF)
endif()

option(CHEESE_USE_SYSTEM_GGML "Use system libggml" OFF)

option(CHEESE_WASM_MEM64 "cheese: use 64-bit memory in WASM builds" ON)

if (EMSCRIPTEN)
    set(BUILD_SHARED_LIBS_DEFAULT OFF)

    # Use 64-bit memory to support backend_get_memory queries
    # TODO: analyze performance impact, see https://spidermonkey.dev/blog/2025/01/15/is-memory64-actually-worth-using
    if (CHEESE_WASM_MEM64)
      add_compile_options("-sMEMORY64=1")
      add_link_options("-sMEMORY64=1")
    endif()
    add_link_options("-sALLOW_MEMORY_GROWTH=1")

    option(CHEESE_WASM_SINGLE_FILE "cheese: embed WASM inside the generated cheese.js" OFF)
    option(CHEESE_BUILD_HTML "cheese: build HTML file" ON)
    if (CHEESE_BUILD_HTML)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()
else()
    if (MINGW)
        set(BUILD_SHARED_LIBS_DEFAULT OFF)
    else()
        set(BUILD_SHARED_LIBS_DEFAULT ON)
    endif()
endif()

option(BUILD_SHARED_LIBS "build shared libraries" ${BUILD_SHARED_LIBS_DEFAULT})

if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if (MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/bigobj>")
    add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/bigobj>")
endif()

if (CHEESE_STANDALONE)
    # enable parallel builds for msbuild
    list(APPEND CMAKE_VS_GLOBALS UseMultiToolTask=true)
    list(APPEND CMAKE_VS_GLOBALS EnforceProcessCountAcrossBuilds=true)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(CHEESE_TOOLS_INSTALL_DEFAULT OFF)
else()
    set(CHEESE_TOOLS_INSTALL_DEFAULT ${CHEESE_STANDALONE})
endif()

#
# option list
#

# debug
option(CHEESE_ALL_WARNINGS           "cheese: enable all compiler warnings"                   ON)
option(CHEESE_ALL_WARNINGS_3RD_PARTY "cheese: enable all compiler warnings in 3rd party libs" OFF)

# build
option(CHEESE_FATAL_WARNINGS "cheese: enable -Werror flag" OFF)

# sanitizers
option(CHEESE_SANITIZE_THREAD    "cheese: enable thread sanitizer"    OFF)
option(CHEESE_SANITIZE_ADDRESS   "cheese: enable address sanitizer"   OFF)
option(CHEESE_SANITIZE_UNDEFINED "cheese: enable undefined sanitizer" OFF)

# utils
option(CHEESE_BUILD_COMMON "cheese: build common utils library" ${CHEESE_STANDALONE})

# extra artifacts
option(CHEESE_BUILD_TESTS    "cheese: build tests"          ${CHEESE_STANDALONE})
option(CHEESE_BUILD_TOOLS    "cheese: build tools"          ${CHEESE_STANDALONE})
option(CHEESE_BUILD_EXAMPLES "cheese: build examples"       ${CHEESE_STANDALONE})
option(CHEESE_BUILD_SERVER   "cheese: build server example" ${CHEESE_STANDALONE})
option(CHEESE_TOOLS_INSTALL  "cheese: install tools"        ${CHEESE_TOOLS_INSTALL_DEFAULT})
option(CHEESE_TESTS_INSTALL  "cheese: install tests"        ON)

# 3rd party libs
option(CHEESE_OPENSSL    "cheese: use openssl to support HTTPS" ON)
option(CHEESE_LLGUIDANCE "cheese-common: include LLGuidance library for structured output in common utils" OFF)


# Required for relocatable CMake package
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build-info.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/common.cmake)

if (NOT DEFINED CHEESE_BUILD_NUMBER)
    set(CHEESE_BUILD_NUMBER        ${BUILD_NUMBER})
endif()
if (NOT DEFINED CHEESE_BUILD_COMMIT)
    set(CHEESE_BUILD_COMMIT        ${BUILD_COMMIT})
endif()
set(CHEESE_INSTALL_VERSION 0.0.${CHEESE_BUILD_NUMBER})

# override ggml options
set(GGML_ALL_WARNINGS   ${CHEESE_ALL_WARNINGS})
set(GGML_FATAL_WARNINGS ${CHEESE_FATAL_WARNINGS})

# change the default for these ggml options
if (NOT DEFINED GGML_CHEESEFILE)
    set(GGML_CHEESEFILE_DEFAULT ON)
endif()

if (NOT DEFINED GGML_CUDA_GRAPHS)
    set(GGML_CUDA_GRAPHS_DEFAULT ON)
endif()

# transition helpers
function (cheese_option_depr TYPE OLD)
    if (${OLD})
        set(NEW "${ARGV2}")
        if(NEW)
            message(${TYPE} "${OLD} is deprecated, use ${NEW} instead")
            set(${NEW} ON PARENT_SCOPE)
        else()
            message(${TYPE} "${OLD} is deprecated and will be ignored")
        endif()
    endif()
endfunction()

cheese_option_depr(FATAL_ERROR CHEESE_CUBLAS              GGML_CUDA)
cheese_option_depr(WARNING     CHEESE_CUDA                GGML_CUDA)
cheese_option_depr(WARNING     CHEESE_METAL               GGML_METAL)
cheese_option_depr(WARNING     CHEESE_METAL_EMBED_LIBRARY GGML_METAL_EMBED_LIBRARY)
cheese_option_depr(WARNING     CHEESE_NATIVE              GGML_NATIVE)
cheese_option_depr(WARNING     CHEESE_RPC                 GGML_RPC)
cheese_option_depr(WARNING     CHEESE_SYCL                GGML_SYCL)
cheese_option_depr(WARNING     CHEESE_SYCL_F16            GGML_SYCL_F16)
cheese_option_depr(WARNING     CHEESE_CANN                GGML_CANN)
cheese_option_depr(WARNING     CHEESE_CURL)

include("cmake/license.cmake")
license_add_file("cheese.cpp" "LICENSE")

#
# 3rd-party
#

# edge target optimization
option(CHEESE_EDGE_TARGET "cheese: optimize for edge devices (disable unused backends)" OFF)
if (ANDROID OR (CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64"))
    set(CHEESE_EDGE_TARGET ON CACHE BOOL "cheese: optimize for edge devices" FORCE)
endif()

# modular backends
option(CHEESE_BACKEND_CUDA   "cheese: enable CUDA backend"   OFF)
option(CHEESE_BACKEND_METAL  "cheese: enable Metal backend"  OFF)
option(CHEESE_BACKEND_VULKAN "cheese: enable Vulkan backend" OFF)

if (CHEESE_EDGE_TARGET)
    set(CHEESE_BACKEND_CUDA   OFF CACHE BOOL "" FORCE)
    set(CHEESE_BACKEND_METAL  OFF CACHE BOOL "" FORCE)
    option(CHEESE_BACKEND_VULKAN "cheese: enable Vulkan backend" ON)
endif()

# Apply to ggml
set(GGML_CUDA   ${CHEESE_BACKEND_CUDA}   CACHE BOOL \"\" FORCE)
set(GGML_METAL  ${CHEESE_BACKEND_METAL}  CACHE BOOL \"\" FORCE)
set(GGML_VULKAN ${CHEESE_BACKEND_VULKAN} CACHE BOOL \"\" FORCE)

if (CHEESE_USE_SYSTEM_GGML)
    message(STATUS "Using system-provided libggml, skipping ggml build")
    find_package(ggml REQUIRED)
    add_library(ggml ALIAS ggml::ggml)
endif()

if (NOT TARGET ggml AND NOT CHEESE_USE_SYSTEM_GGML)
    set(GGML_BUILD_NUMBER ${CHEESE_BUILD_NUMBER})
    set(GGML_BUILD_COMMIT ${CHEESE_BUILD_COMMIT})
    add_subdirectory(ggml)
    # ... otherwise assume ggml is added by a parent CMakeLists.txt
endif()

# palloc
option(CHEESE_PALLOC "cheese: use palloc (mimalloc-based) allocator" ON)
if (CHEESE_PALLOC)
    set(PA_BUILD_TESTS OFF CACHE BOOL "Build palloc tests" FORCE)
    add_subdirectory(third_party/palloc)
endif()

# contextsqueezer
option(CHEESE_CONTEXT_SQUEEZER "cheese: use contextsqueezer for token squeezing" ON)
if (CHEESE_CONTEXT_SQUEEZER)
    add_subdirectory(third_party/contextsqueezer/native)
endif()

#
# build the library
#

add_subdirectory(src)

#
# utils, programs, examples and tests
#

if (CHEESE_BUILD_COMMON)
    add_subdirectory(common)
    add_subdirectory(vendor/cpp-httplib)
endif()

if (CHEESE_BUILD_COMMON AND CHEESE_BUILD_TESTS AND NOT CMAKE_JS_VERSION)
    include(CTest)
    add_subdirectory(tests)
endif()

if (CHEESE_BUILD_COMMON AND CHEESE_BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(pocs)
endif()

if (CHEESE_BUILD_COMMON AND CHEESE_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# Automatically add all files from the 'licenses' directory
file(GLOB EXTRA_LICENSES "${CMAKE_SOURCE_DIR}/licenses/LICENSE-*")

foreach(FILE_PATH ${EXTRA_LICENSES})
    get_filename_component(FILE_NAME "${FILE_PATH}" NAME)
    string(REGEX REPLACE "^LICENSE-" "" NAME "${FILE_NAME}")
    license_add_file("${NAME}" "${FILE_PATH}")
endforeach()

if (CHEESE_BUILD_COMMON)
    license_generate(common)
endif()

#
# install
#

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CHEESE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Location of header  files")
set(CHEESE_LIB_INSTALL_DIR     ${CMAKE_INSTALL_LIBDIR}     CACHE PATH "Location of library files")
set(CHEESE_BIN_INSTALL_DIR     ${CMAKE_INSTALL_BINDIR}     CACHE PATH "Location of binary  files")

set(CHEESE_PUBLIC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cheese.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cheese-cpp.h)

set_target_properties(cheese
    PROPERTIES
        PUBLIC_HEADER "${CHEESE_PUBLIC_HEADERS}")

install(TARGETS cheese LIBRARY PUBLIC_HEADER)

configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cheese-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cheese-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cheese
    PATH_VARS CHEESE_INCLUDE_INSTALL_DIR
              CHEESE_LIB_INSTALL_DIR
              CHEESE_BIN_INSTALL_DIR )

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/cheese-version.cmake
    VERSION ${CHEESE_INSTALL_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cheese-config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/cheese-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cheese)

install(
    FILES convert_hf_to_gguf.py
    PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE
    DESTINATION ${CMAKE_INSTALL_BINDIR})

configure_file(cmake/cheese.pc.in
        "${CMAKE_CURRENT_BINARY_DIR}/cheese.pc"
        @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cheese.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
