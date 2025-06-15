#include "huffman_tree.h"
#include <queue>

HuffmanTree::Node::Node(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}

bool HuffmanTree::Node::operator>(const Node& other) const {
    return freq > other.freq;
}

HuffmanTree::HuffmanTree() : root(nullptr) {}

void HuffmanTree::build(std::map<char, int>& frequencies) {
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    for (const auto& pair : frequencies) {
        pq.push(Node(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        Node* left = new Node(pq.top()); pq.pop();
        Node* right = new Node(pq.top()); pq.pop();
        Node* internal = new Node('\0', left->freq + right->freq);
        internal->left = left;
        internal->right = right;
        pq.push(*internal);
    }

    root = new Node(pq.top()); pq.pop();
    generateCodes(root, "");
}

std::map<char, std::string> HuffmanTree::getCodes() const {
    return codes;
}

void HuffmanTree::generateCodes(Node* node, const std::string& code) {
    if (node == nullptr) return;

    if (node->ch != '\0') {
        codes[node->ch] = code;
    }

    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

HuffmanTree::~HuffmanTree() {
    deleteTree(root);
}

void HuffmanTree::deleteTree(Node* node) {
    if (node == nullptr) return;

    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}

HuffmanTree::Node* HuffmanTree::getRoot() const {
    return root;
}
