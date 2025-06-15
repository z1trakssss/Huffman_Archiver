#include "doctest.h"
#include "../src/huffman.h"
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>

namespace fs = std::filesystem;

// Вспомогательная функция для безопасного удаления файлов
void cleanup_files(const std::vector<std::string>& files) {
    for (const auto& file : files) {
        try {
            if (fs::exists(file)) {
                fs::remove(file);
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Не удалось удалить файл " << file << ": " << e.what() << "\n";
        }
    }
}

TEST_CASE("Huffman compression and decompression") {
    // Подготовка тестового файла
    std::string test_input = "test_input.txt";
    std::string test_compressed = "test_compressed.huff";
    std::string test_decompressed = "test_decompressed.txt";

    std::ofstream out(test_input);
    out << "hello world";
    out.close();

    HuffmanArchiver archiver;

    SUBCASE("Положительный: Сжатие и разархивация валидного файла") {
        archiver.compress(test_input, test_compressed);
        CHECK(fs::exists(test_compressed));

        archiver.decompress(test_compressed, test_decompressed);
        CHECK(fs::exists(test_decompressed));

        std::ifstream in_decomp(test_decompressed);
        std::string content((std::istreambuf_iterator<char>(in_decomp)), std::istreambuf_iterator<char>());
        in_decomp.close(); // Явно закрываем файл
        CHECK(content == "hello world");

        cleanup_files({test_input, test_compressed, test_decompressed});
    }

    SUBCASE("Отрицательный: Сжатие пустого файла") {
        std::ofstream empty_file(test_input, std::ios::trunc);
        empty_file.close();

        CHECK_THROWS_AS(archiver.compress(test_input, test_compressed), std::runtime_error);

        cleanup_files({test_input, test_compressed});
    }

    SUBCASE("Отрицательный: Сжатие несуществующего файла") {
        CHECK_THROWS_AS(archiver.compress("non_existent.txt", test_compressed), std::runtime_error);
        cleanup_files({test_compressed});
    }
}

TEST_CASE("Huffman decompress with frequency table output") {
    // Подготовка тестовых файлов
    std::string test_input = "test_input.txt";
    std::string test_compressed = "test_compressed.huff";
    std::string test_decompressed = "test_decompressed.txt";
    std::string test_freq = "test_decompressed_freq.txt";

    // Подготовка исходного файла
    std::ofstream out(test_input);
    out << "hello world";
    out.close();

    HuffmanArchiver archiver;

    SUBCASE("Положительный: Разархивация с сохранением таблицы частот") {
        // Сжимаем файл
        archiver.compress(test_input, test_compressed);
        CHECK(fs::exists(test_compressed));

        // Разархивируем с сохранением таблицы частот
        archiver.decompress(test_compressed, test_decompressed, true);
        CHECK(fs::exists(test_decompressed));
        CHECK(fs::exists(test_freq));

        // Проверяем содержимое разархивированного файла
        std::ifstream in_decomp(test_decompressed);
        std::string content((std::istreambuf_iterator<char>(in_decomp)), std::istreambuf_iterator<char>());
        in_decomp.close();
        CHECK(content == "hello world");

        // Проверяем содержимое файла частот
        std::ifstream in_freq(test_freq);
        std::map<char, uint64_t> freq_map;
        std::string line;
        while (std::getline(in_freq, line)) {
            if (line.empty()) continue;
            char symbol = line[line.find("Symbol: ") + 8];
            uint64_t freq = std::stoull(line.substr(line.find("Frequency: ") + 11));
            freq_map[symbol] = freq;
        }
        in_freq.close();

        // Ожидаемая таблица частот для "hello world"
        std::map<char, uint64_t> expected_freq = {
            {'h', 1}, {'e', 1}, {'l', 3}, {'o', 2}, {' ', 1}, {'w', 1}, {'r', 1}, {'d', 1}
        };
        CHECK(freq_map == expected_freq);

        cleanup_files({test_input, test_compressed, test_decompressed, test_freq});
    }

    SUBCASE("Отрицательный: Разархивация поврежденного архива с сохранением таблицы частот") {
        // Создаем поврежденный архив: только заголовок с некорректной таблицей частот
        std::ofstream corrupted_out(test_compressed, std::ios::binary);
        uint32_t size = 1; // Указываем, что есть одна запись
        corrupted_out.write(reinterpret_cast<char*>(&size), sizeof(size));
        unsigned char symbol = 'a';
        uint64_t freq = 5;
        corrupted_out.write(reinterpret_cast<char*>(&symbol), 1);
        corrupted_out.write(reinterpret_cast<char*>(&freq), 4);
        corrupted_out.close();

        CHECK_THROWS_AS(archiver.decompress(test_compressed, test_decompressed, true), std::runtime_error);
        CHECK_FALSE(fs::exists(test_freq)); // Убеждаемся, что файл частот не создан

        cleanup_files({test_compressed, test_decompressed, test_freq});
    }
}

TEST_CASE("Huffman tree building") {
    // Подготовка тестовых файлов
    std::string test_input = "test_input.txt";
    std::string test_compressed = "test_compressed.huff";

    HuffmanArchiver archiver;

    SUBCASE("Отрицательный: Построение дерева для пустого файла") {
        // Создаем пустой файл
        std::ofstream empty_file(test_input, std::ios::trunc);
        empty_file.close();

        // Ожидаем исключение при сжатии пустого файла
        CHECK_THROWS_AS(archiver.compress(test_input, test_compressed), std::runtime_error);

        // Проверяем, что коды Хаффмана не созданы
        const auto& codes = archiver.getHuffmanCodes();
        CHECK(codes.empty());

        cleanup_files({test_input, test_compressed});
    }
}