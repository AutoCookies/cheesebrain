set(TARGET cheese-cvector-generator)
add_executable(${TARGET} cvector-generator.cpp pca.hpp)
target_link_libraries(${TARGET} PRIVATE common cheese ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PRIVATE cxx_std_17)

if(CHEESE_TOOLS_INSTALL)
    install(TARGETS ${TARGET} RUNTIME)
endif()
