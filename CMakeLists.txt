cmake_minimum_required(VERSION 3.24)
project(mal)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(pcre2-10.40)

option(WITH_THREADS "With threading support" ON)

if (WITH_THREADS)
    find_package(Threads REQUIRED)
endif()

add_library(linenoise STATIC src/linenoise.c src/linenoise.h)
add_library(mal-lib STATIC src/mal.cpp src/mal.h src/reader.cpp src/printer.cpp src/variant_overload.hpp src/hashes.h src/evaluator.cpp src/functions.h src/core.cpp)
if (WITH_THREADS)
    target_link_libraries(mal-lib PUBLIC Threads::Threads)
    target_compile_definitions(mal-lib PUBLIC -DTHREADING=1)
endif()
target_link_libraries(mal-lib PUBLIC linenoise pcre2-posix-static)
target_include_directories(mal-lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/pcre2-10.40)

add_executable(mal src/main.cpp)
target_link_libraries(mal PUBLIC mal-lib)

add_executable(mal-tests test/main.cpp test/basic_repl.cpp test/mal_data.cpp test/eval.cpp test/mal_core.cpp mal_files_out)
target_link_libraries(mal-tests PUBLIC mal-lib)
target_include_directories(mal-tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(
        mal-files
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test/test.txt ${CMAKE_CURRENT_BINARY_DIR}/test.txt
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/test/test.lsp ${CMAKE_CURRENT_BINARY_DIR}/test.lsp
        BYPRODUCTS mal_files_out
)

ENABLE_TESTING()
ADD_TEST(mal_tests mal-tests)
