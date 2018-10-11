cmake_minimum_required(VERSION 3.10)
project(jhlex)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(./regex_tree/)

add_executable(jhlex main.cpp)
add_executable(test_code test_code/printSomething.cpp regex_tree/regex_tree.cpp)
add_executable(test_code_true test_code/testcode.cpp regex_tree/regex_tree.cpp)
add_executable(test_lex lex/lex_test.cpp lex/LexFile.h lex/LexFile.cpp
        regex_tree/regex_tree.cpp)
