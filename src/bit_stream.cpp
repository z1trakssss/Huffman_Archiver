#include "bit_stream.hpp"

BitStream::BitStream(const std::string& filename, std::ios::openmode mode)
    : file(filename, mode), bitBuffer(0), bitCount(0) {
}

bool BitStream::readBit() {
    if (bitCount == 0) {
        bitBuffer = file.get();
        bitCount = 8;
    }
    bool bit = (bitBuffer >> (bitCount - 1)) & 1;
    bitCount--;
    return bit;
}

unsigned int BitStream::readBits(int n) {
    unsigned int result = 0;
    for (int i = 0; i < n; i++) {
        result = (result << 1) | readBit();
    }
    return result;
}

BitStream::~BitStream() {
    if (file.is_open()) {
        file.close();
    }
}

BitWriter::BitWriter(const std::string& filename)
    : file(filename, std::ios::binary), bitBuffer(0), bitCount(0) {
}

void BitWriter::writeBit(bool bit) {
    bitBuffer = (bitBuffer << 1) | bit;
    bitCount++;
    if (bitCount == 8) {
        file.put(bitBuffer);
        bitBuffer = 0;
        bitCount = 0;
    }
}

void BitWriter::writeBits(unsigned int bits, int n) {
    for (int i = n - 1; i >= 0; i--) {
        bool bit = (bits >> i) & 1;
        writeBit(bit);
    }
}

BitWriter::~BitWriter() {
    if (bitCount > 0) {
        file.put(bitBuffer << (8 - bitCount));
    }
    if (file.is_open()) {
        file.close();
    }
}
