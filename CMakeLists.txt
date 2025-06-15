cmake_minimum_required(VERSION 3.10)

project(HuffmanArchiver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src)

set(SOURCES
    main.cpp
    src/archiver.cpp
    src/huffman_tree.cpp
    src/bit_stream.cpp
)

add_executable(archiver ${SOURCES})
