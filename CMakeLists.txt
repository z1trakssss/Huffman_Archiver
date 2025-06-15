cmake_minimum_required(VERSION 3.10)
project(HuffmanArchiver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основной исполняемый файл
add_executable(huffman main.cpp src/huffman.cpp)
target_include_directories(huffman PRIVATE src)
target_link_libraries(huffman PRIVATE stdc++fs)

# Тестовый исполняемый файл
add_executable(huffman_tests tests/test_huffman.cpp src/huffman.cpp)
target_include_directories(huffman_tests PRIVATE src tests)
target_link_libraries(huffman_tests PRIVATE stdc++fs)
add_test(NAME HuffmanTests COMMAND huffman_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})