cmake_minimum_required(VERSION 3.10)
project(ArithmeticTranslator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_library(translator_lib src/translator.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

function(add_class_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE translator_lib GTest::gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_class_test(test_vector tests/test_vector.cpp)
add_class_test(test_stack tests/test_stack.cpp)
add_class_test(test_queue tests/test_queue.cpp)
add_class_test(test_translator tests/test_translator.cpp)