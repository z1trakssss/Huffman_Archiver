#ifndef ARCHIVER_HPP
#define ARCHIVER_HPP

#include <map>      
#include <string>   
#include <vector>

class Archiver {
public:
    void compress(const std::string& archiveName, const std::vector<std::string>& files);
    void decompress(const std::string& archiveName);

private:
    void countFrequencies(const std::string& filename, std::map<char, int>& frequencies);
};

#endif // ARCHIVER_HPP
