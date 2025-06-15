#ifndef HUFFMAN_TREE_HPP
#define HUFFMAN_TREE_HPP

#include <map>
#include <string>

class HuffmanTree {
public:
    struct Node {
        char ch;
        int freq;
        Node* left;
        Node* right;
        Node(char c, int f);
        bool operator>(const Node& other) const;
    };

    HuffmanTree();
    void build(std::map<char, int>& frequencies);
    std::map<char, std::string> getCodes() const;
    ~HuffmanTree();

private:
    Node* root;
    std::map<char, std::string> codes;

    void generateCodes(Node* node, const std::string& code);
    void deleteTree(Node* node);

public:
    Node* getRoot() const;

};

#endif // HUFFMAN_TREE_HPP
