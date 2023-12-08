#pragma once

#include <fstream>
#include <string>
#include <vector>
#include "bitstream.h"
#include "node.h"


// Computes vector of length 257 storing frequencies for Huffman encoding of given plaintext
std::vector<int> compute_frequencies(std::ifstream& plaintext);

// Returns root of newly computed Huffman tree for given frequencies
Node* build_tree(const std::vector<int>& freq);

// Encodes the given (sub)tree within the given bitstream
void encode_tree(OutBitStream& bitstream, Node* subtree);

// Stores the '0'/'1' string associated with every leaf in the given subtree,
// presuming given coding for the path to reach the subtree
void fill_codebook(std::vector<std::string>& codebook, Node* subtree, std::string& path);

// Inserts the message encoding for plaintext into the bitstream using the codebook
void encode_message(std::ifstream& plaintext, OutBitStream& bitstream, const std::vector<std::string>& codebook);


