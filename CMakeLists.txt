cmake_minimum_required(VERSION 3.10)
project(HuffmanArchiver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_executable(huffman
    main.cpp
    src/huffman.cpp
)

target_include_directories(huffman PRIVATE src)

add_executable(huffman_tests
    tests/test_huffman.cpp
    src/huffman.cpp
)

target_include_directories(huffman_tests PRIVATE src)

target_compile_definitions(huffman_tests PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

add_test(NAME HuffmanTests COMMAND huffman_tests)