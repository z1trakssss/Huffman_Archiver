cmake_minimum_required(VERSION 3.10)
project(HuffmanArchiver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включаем тестирование
enable_testing()

# Добавляем основной исполняемый файл
add_executable(huffman
    main.cpp
    src/huffman.cpp
)

# Указываем директории с заголовочными файлами
target_include_directories(huffman PRIVATE src)

# Добавляем тестовый исполняемый файл
add_executable(huffman_tests
    tests/test_huffman.cpp
    src/huffman.cpp
)

# Указываем директории с заголовочными файлами для тестов
target_include_directories(huffman_tests PRIVATE src)

# Указываем, что Doctest реализует main
target_compile_definitions(huffman_tests PRIVATE DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN)

# Регистрируем тесты для ctest
add_test(NAME HuffmanTests COMMAND huffman_tests)