cmake_minimum_required(VERSION 3.12)
project(cheese-simple-cmake-pkg)

set(TARGET cheese-simple-cmake-pkg)

find_package(Cheese REQUIRED)

add_executable(${TARGET} ${CMAKE_CURRENT_LIST_DIR}/../simple/simple.cpp)
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE cheese ggml::all ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PRIVATE cxx_std_17)
