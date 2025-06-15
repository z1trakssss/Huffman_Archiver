#include "huffman.h"
#include <fstream>
#include <queue>
#include <filesystem>

namespace fs = std::filesystem;

// Implementation of node constructors
Node::Node(unsigned char s, uint64_t f) : symbol(s), freq(f), left(nullptr), right(nullptr) {}
Node::Node(std::shared_ptr<Node> l, std::shared_ptr<Node> r) 
    : symbol(0), freq(l->freq + r->freq), left(l), right(r) {}

// Comparator for priority queue
struct Compare {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
        return a->freq > b->freq;
    }
};

// Implementation of HuffmanArchiver methods
void HuffmanArchiver::buildFrequencyTable(const std::string& input_file) {
    std::ifstream in(input_file, std::ios::binary);
    if (!in) throw std::runtime_error("Failed to open input file");
    unsigned char byte;
    while (in.read(reinterpret_cast<char*>(&byte), 1)) {
        freq_table[byte]++;
    }
}

void HuffmanArchiver::buildHuffmanTree() {
    std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, Compare> pq;
    for (const auto& pair : freq_table) {
        pq.push(std::make_shared<Node>(pair.first, pair.second));
    }
    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        pq.push(std::make_shared<Node>(left, right));
    }
    root = pq.empty() ? nullptr : pq.top();
}

void HuffmanArchiver::buildHuffmanCodes(const std::shared_ptr<Node>& node, const std::string& code) {
    if (!node) return;
    if (!node->left && !node->right) {
        huffman_codes[node->symbol] = code.empty() ? "0" : code;
    }
    buildHuffmanCodes(node->left, code + "0");
    buildHuffmanCodes(node->right, code + "1");
}

void HuffmanArchiver::writeFrequencyTable(std::ofstream& out) {
    uint32_t size = freq_table.size();
    out.write(reinterpret_cast<char*>(&size), sizeof(size));
    for (const auto& pair : freq_table) {
        out.write(reinterpret_cast<const char*>(&pair.first), 1);
        uint64_t freq = pair.second;
        out.write(reinterpret_cast<char*>(&freq), sizeof(freq));
    }
}

void HuffmanArchiver::readFrequencyTable(std::ifstream& in) {
    uint32_t size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    if (!in) throw std::runtime_error("Failed to read frequency table size");
    for (uint32_t i = 0; i < size; ++i) {
        unsigned char symbol;
        uint64_t freq;
        in.read(reinterpret_cast<char*>(&symbol), 1);
        if (!in) throw std::runtime_error("Corrupted frequency table: failed to read symbol");
        in.read(reinterpret_cast<char*>(&freq), sizeof(freq));
        if (!in) throw std::runtime_error("Corrupted frequency table: failed to read frequency");
        freq_table[symbol] = freq;
    }
}

/**
 * @brief Compresses the input file into an archive using Huffman algorithm
 * @param input_file Path to the input file
 * @param output_file Path to the output archive
 * @throws std::runtime_error If files cannot be opened or compression fails
 */
void HuffmanArchiver::compress(const std::string& input_file, const std::string& output_file) {
    freq_table.clear();
    huffman_codes.clear();
    buildFrequencyTable(input_file);
    if (freq_table.empty()) throw std::runtime_error("Input file is empty");
    buildHuffmanTree();
    buildHuffmanCodes(root, "");

    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);
    if (!in || !out) throw std::runtime_error("Error opening files");

    writeFrequencyTable(out);

    std::string buffer;
    unsigned char byte;
    while (in.read(reinterpret_cast<char*>(&byte), 1)) {
        buffer += huffman_codes[byte];
        while (buffer.size() >= 8) {
            unsigned char out_byte = 0;
            for (int i = 0; i < 8; ++i) {
                out_byte |= (buffer[i] == '1' ? 1 : 0) << (7 - i);
            }
            out.write(reinterpret_cast<char*>(&out_byte), 1);
            buffer.erase(0, 8);
        }
    }
    if (!buffer.empty()) {
        unsigned char out_byte = 0;
        for (size_t i = 0; i < buffer.size(); ++i) {
            out_byte |= (buffer[i] == '1' ? 1 : 0) << (7 - i);
        }
        out.write(reinterpret_cast<char*>(&out_byte), 1);
        unsigned char padding = 8 - buffer.size();
        out.write(reinterpret_cast<char*>(&padding), 1);
    } else {
        unsigned char padding = 0;
        out.write(reinterpret_cast<char*>(&padding), 1);
    }
}

void HuffmanArchiver::decompress(const std::string& input_file, const std::string& output_file, bool write_freq) {
    freq_table.clear();
    std::ifstream in(input_file, std::ios::binary);
    std::ofstream out(output_file, std::ios::binary);
    if (!in || !out) throw std::runtime_error("Error opening files");

    readFrequencyTable(in);
    if (freq_table.empty()) throw std::runtime_error("Archive is empty or corrupted");
    buildHuffmanTree();

    if (write_freq) {
        std::ofstream freq_out(fs::path(output_file).stem().string() + "_freq.txt");
        for (const auto& pair : freq_table) {
            char symbol = static_cast<char>(pair.first);
            freq_out << "Symbol: " << symbol << ", Frequency: " << pair.second << "\n";
        }
    }

    unsigned char padding;
    in.seekg(-1, std::ios::end);
    in.read(reinterpret_cast<char*>(&padding), 1);
    in.seekg(freq_table.size() * 9 + 4, std::ios::beg);

    std::shared_ptr<Node> current = root;
    unsigned char byte;
    std::streampos file_size = fs::file_size(input_file);
    std::streampos data_start = freq_table.size() * 9 + 4; // Начало данных
    std::streampos data_end = file_size - std::streamoff(1); // Конец данных (исключая padding)
    uint64_t total_bits = (data_end - data_start) * 8 - padding; // Общее количество бит для обработки
    uint64_t processed_bits = 0;

    while (in.read(reinterpret_cast<char*>(&byte), 1) && in.tellg() <= data_end) {
        for (int i = 7; i >= 0 && processed_bits < total_bits; --i) {
            bool bit = (byte >> i) & 1;
            current = bit ? current->right : current->left;
            if (!current->left && !current->right) {
                out.write(reinterpret_cast<char*>(&current->symbol), 1);
                current = root;
            }
            processed_bits++;
        }
    }
}