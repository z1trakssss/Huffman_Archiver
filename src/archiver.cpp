#include "archiver.h"
#include "huffman_tree.h"
#include "bit_stream.h"
#include <fstream>
#include <cstdint>
#include <iostream>


void Archiver::compress(const std::string& archiveName, const std::vector<std::string>& files) {
    if (files.empty()) return;

    std::map<char, int> frequencies;
    const std::string& filename = files[0];

    countFrequencies(filename, frequencies);

    HuffmanTree tree;
    tree.build(frequencies);
    auto codes = tree.getCodes();

    std::ifstream input(filename, std::ios::binary);

    BitWriter writer(archiveName);

    writer.writeBits(frequencies.size(), 9);

    for (const auto& [ch, freq] : frequencies) {
        writer.writeBits(static_cast<unsigned int>(ch), 9);

        for (int i = 0; i < 4; ++i) {
            writer.writeBits((freq >> (i * 8)) & 0xFF, 8);
        }
    }

    input.seekg(0, std::ios::end);
    uint64_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);
    
    for (int i = 0; i < 8; ++i) {
        writer.writeBits((fileSize >> (i * 8)) & 0xFF, 8);
    }

    char c;
    while (input.get(c)) {
        const std::string& code = codes[c];
        for (char bitChar : code) {
            writer.writeBit(bitChar == '1');
        }
    }

    input.close();
}


void Archiver::decompress(const std::string& archiveName) {
    BitStream reader(archiveName, std::ios::binary);

    unsigned int uniqueSymbols = reader.readBits(9);
    std::map<char, int> frequencies;

    for (unsigned int i = 0; i < uniqueSymbols; i++) {
        char c = static_cast<char>(reader.readBits(9));
        int freq = 0;
        for (int b = 0; b < 4; b++) {
            freq |= (reader.readBits(8) << (b * 8));
        }
        frequencies[c] = freq;
    }

    uint64_t totalSymbols = 0;
    for (int b = 0; b < 8; b++) {
        totalSymbols |= (static_cast<uint64_t>(reader.readBits(8) << (b * 8)));
    }

    HuffmanTree tree;
    tree.build(frequencies);

    std::ofstream output("output.bin", std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "Error: Cannot open output.bin for writing.\n";
        return;
    }

    for (uint64_t i = 0; i < totalSymbols; i++) {
        HuffmanTree::Node* node = tree.getRoot();
        while (node->ch == '\0') {
            bool bit = reader.readBit();
            node = bit ? node->right : node->left;
        }
        output.put(node->ch);
    }

    output.close();
    std::cout << "Decompression finished. File saved as output.bin\n";
}


void Archiver::countFrequencies(const std::string& filename, std::map<char, int>& frequencies) {
    std::ifstream file(filename, std::ios::binary);
    char c;
    while (file.get(c)) {
        frequencies[c]++;
    }
    file.close();
}
