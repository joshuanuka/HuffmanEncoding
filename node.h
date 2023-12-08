#pragma once

class Node {
  public:
    Node* left;   // pointer to left child (possibly nullptr)
    Node* right;  // pointer to right child (possibly nullptr)
    int   code;   // 9-bit code (only relevant for a leaf)

    Node(int code = -1, Node* left = nullptr, Node* right = nullptr)
        : left{left}, right{right}, code{code} {}
};
