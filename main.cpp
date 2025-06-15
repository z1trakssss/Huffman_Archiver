#include <iostream>
#include <string>
#include <filesystem>
#include "src/huffman.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <command> <file> [output_file]\n";
        std::cerr << "Commands: compress, decompress, decompress_with_freq\n";
        return 1;
    }

    std::string command = argv[1];
    std::string input_file = argv[2];
    std::string output_file = argc > 3 ? argv[3] : (command == "compress" ? input_file + ".huff" : fs::path(input_file).stem().string() + "_decomp" + fs::path(input_file).extension().string());

    HuffmanArchiver archiver;
    try {
        if (command == "compress") {
            archiver.compress(input_file, output_file);
            std::cout << "Compression completed: " << output_file << "\n";
        } else if (command == "decompress") {
            archiver.decompress(input_file, output_file);
            std::cout << "Decompression completed: " << output_file << "\n";
        } else if (command == "decompress_with_freq") {
            archiver.decompress(input_file, output_file, true);
            std::cout << "Decompression with frequencies completed: " << output_file << "\n";
        } else {
            std::cerr << "Unknown command: " << command << "\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}