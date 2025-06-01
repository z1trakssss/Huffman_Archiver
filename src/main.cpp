#include "archiver.hpp"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: archiver -c <archive_name> <file1> [file2 ...]" << std::endl;
        return 111;
    }

    Archiver archiver;
    std::string archiveName = argv[2];
    std::vector<std::string> files;

    for (int i = 3; i < argc; i++) {
        files.push_back(argv[i]);
    }

    if (std::string(argv[1]) == "-c") {
        archiver.compress(archiveName, files);
    }
    else if (std::string(argv[1]) == "-d") {
        archiver.decompress(archiveName);
    }

    return 0;
}
