#ifndef BIT_STREAM_HPP
#define BIT_STREAM_HPP

#include <fstream>

class BitStream {
public:
    BitStream(const std::string& filename, std::ios::openmode mode);
    bool readBit();
    unsigned int readBits(int n);
    ~BitStream();

private:
    std::ifstream file;
    unsigned char bitBuffer;
    int bitCount;
};

class BitWriter {
public:
    BitWriter(const std::string& filename);
    void writeBit(bool bit);
    void writeBits(unsigned int bits, int n);
    ~BitWriter();

private:
    std::ofstream file;
    unsigned char bitBuffer;
    int bitCount;
};

#endif // BIT_STREAM_HPP
