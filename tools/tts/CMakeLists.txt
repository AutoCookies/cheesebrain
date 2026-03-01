set(TARGET cheese-tts)
add_executable(${TARGET} tts.cpp)
target_link_libraries(${TARGET} PRIVATE cheese common ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PRIVATE cxx_std_17)

if(CHEESE_TOOLS_INSTALL)
    install(TARGETS ${TARGET} RUNTIME)
endif()
