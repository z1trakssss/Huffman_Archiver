#include "archiver.hpp"
#include "huffman_tree.hpp"
#include "bit_stream.hpp"
#include <fstream>
#include <cstdint>
#include <iostream>


void Archiver::compress(const std::string& archiveName, const std::vector<std::string>& files) {
    if (files.empty()) return;

    std::map<char, int> frequencies;
    const std::string& filename = files[0];  // �������� ������ � ����� ������

    countFrequencies(filename, frequencies);

    HuffmanTree tree;
    tree.build(frequencies);
    auto codes = tree.getCodes();

    // ��������� ���� ��� ������ �������� ������
    std::ifstream input(filename, std::ios::binary);

    // ��������� ���� ������ ��� ������
    BitWriter writer(archiveName);

    // ���������� ���������� ���������� �������� (9 ���)
    writer.writeBits(frequencies.size(), 9);

    // ���������� ������� � �� ������� (������ 9 ��� + ������� 32 ����)
    for (const auto& [ch, freq] : frequencies) {
        writer.writeBits(static_cast<unsigned int>(ch), 9);
        // ������ 32 ��� ������� (4 �����)
        for (int i = 3; i >= 0; --i) {
            writer.writeBits((freq >> (i * 8)) & 0xFF, 8);
        }
    }

    // �������� ����� ������ ����� � ������
    input.seekg(0, std::ios::end);
    uint64_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);

    // ���������� ������ ����� � 8 ���� (64 ����)
    for (int i = 7; i >= 0; --i) {
        writer.writeBits((fileSize >> (i * 8)) & 0xFF, 8);
    }

    // �������� ������
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
            freq = (freq << 8) | reader.readBits(8);
        }
        frequencies[c] = freq;
    }

    uint64_t totalSymbols = 0;
    for (int b = 0; b < 8; b++) {
        totalSymbols = (totalSymbols << 8) | reader.readBits(8);
    }

    HuffmanTree tree;
    tree.build(frequencies);

    std::ofstream output("output.bin", std::ios::binary);
    if (!output.is_open()) {
        std::cerr << "������ �������� ����� output.bin ��� ������\n";
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
    std::cout << "���� ���������� � output.bin\n";
}


void Archiver::countFrequencies(const std::string& filename, std::map<char, int>& frequencies) {
    std::ifstream file(filename, std::ios::binary);
    char c;
    while (file.get(c)) {
        frequencies[c]++;
    }
    file.close();
}
