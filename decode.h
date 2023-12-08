#pragma once

#include "bitstream.h"
#include "node.h"

// read sufficient bits from the input to reconstruct the tree
// and return a pointer to the root node.
// (do not read any message bits from the stream)
Node* build_tree(InBitStream& input);


// translate the message from the input bitstream
// and write the decoded message to given output file
// code is represented by the rooted tree
void decode_message(InBitStream& input, std::ofstream& output, Node* root);
