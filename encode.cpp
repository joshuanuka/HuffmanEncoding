#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <set>
#include <string>
using namespace std;

#include "bitstream.h"
#include "decode.h"
#include "node.h"
#include "student_encode.h"

void draw_tree(Node* root, const string& filename="tree.png") {
    std::ofstream out("tree.dot");
    out << "digraph {" << std::endl;

    if (root != nullptr) {
	// Follow all possible links and expand each node once
	// This will work even if some of the links are messed up.
	std::set<Node *> expanded;
	std::list<Node *> toExpand;
	toExpand.push_back(root);
	while (!toExpand.empty()) {
	    Node *current = toExpand.front();
	    toExpand.pop_front();
	    if (expanded.find(current) == expanded.end()) {
		expanded.insert(current);
		if (current->left != nullptr)
		    toExpand.push_back(current->left);
		if (current->right != nullptr)
		    toExpand.push_back(current->right);
	    }
	}

	// Draw labels
        //	out << "root -> node" << root << ";" << std::endl;
	typename std::set<Node *>::iterator nodes;
	for (nodes=expanded.begin(); nodes!=expanded.end(); ++nodes) {
	    out << "node" << *nodes << " ";
	    out << "[shape=record,label=\"{";
            if ((*nodes)->code == -1)
                out << " " << " | ";
            else if ((*nodes)->code == 256)
                out << "EOM" << " | ";
            else if (char((*nodes)->code) == '\n')
                out << "\n" << " | ";
            else
                out << "'" << char((*nodes)->code) << "' | ";
	    out << "{ <left> left | <right> right } }\"];" << std::endl;
	}

	// Draw links
	for (nodes=expanded.begin(); nodes!=expanded.end(); ++nodes) {
	    if ((*nodes)->left)
		out << "node" << *nodes << ":left:s -> node" <<
		    (*nodes)->left << ";" << std::endl;
	    if ((*nodes)->right)
		out << "node" << *nodes << ":right:s -> node" <<
		    (*nodes)->right << ";" << std::endl;
	}

        //	if (root != nullptr)
        //	    out << "root -> node" << root << ";" << std::endl;
    }
    else {
	out << "zero [shape=none, label=\"0\"];"  << std::endl;
	out << "root -> zero;" << std::endl;
    }

    out << "}" << std::endl;

    std::string command = "dot tree.dot -T png -o " + filename;
    pclose(popen(command.c_str(), "r"));
}


int main() {

    //------- get original filename ---------
    string filename;
    ifstream raw;
    while (!raw.is_open()) {
        cout << "Original filename: ";
        cin >> filename;
        raw.open(filename, ios::binary | ios::in);
        if (!raw.is_open())
            cout << "Unable to open file: " << filename << endl;
    }

    size_t slash{filename.find("/")};
    if (slash != string::npos)
        filename = filename.substr(1+slash);

    OutBitStream encoded;
    encoded.open(filename+".myzip");


    //------- compute_frequencies ---------
    cout << endl << "About to call compute_frequencies..." << endl;
    vector<int> counts = compute_frequencies(raw);
    cout << "...call to compute_frequencies is complete" << endl;
    cout << "nonzero frequencies include the following" << endl;
    cout << "count  ASCII  char" << endl;
    for (int c=0; c < 257; c++)
        if (counts[c] > 0) {
            cout << setw(5) << counts[c] << "  " << setw(4) << c << "   ";
            if (c == 256) cout << "EOM";
            else if (c == 10) cout << "'\\n'";
            else if (c == 13) cout << "'\\r'";
            else cout << "'" << char(c) << "'";
            cout << endl;
        }

    // reset input file to beginning to allow a second iteration
    raw.clear();
    raw.seekg(0);
    
    //------- build_tree ---------
    cout << endl << "About to call build_tree..." << endl;
    Node* root{build_tree(counts)};
    cout << "...call to build_tree is complete" << endl;

    if (root != nullptr) {
        draw_tree(root,"tree1.png");
        cout << "An image of the tree returned by build_tree should be shown in tree1.png," << endl;
    }
    

    //------- encode_tree ---------
    cout << endl << "About to call encode_tree..." << endl;
    encode_tree(encoded, root);
    cout << "...call to encode_tree is complete" << endl;


    //------- fill_codebook ---------
    cout << endl << "About to call fill_codebook..." << endl;
    vector<string> codebook(257,"");
    string path;
    fill_codebook(codebook, root, path);
    cout << "...call to fill_codebook complete" << endl;
    cout << "nonempty codes include the following" << endl;
    cout << "ASCII#  char  Huffman code" << endl;
    for (int c=0; c < 257; c++)
        if (codebook[c] != "") {
            cout << setw(5) << c << "   ";
            if (c == 256) cout << "EOM ";
            else if (c == 10) cout << "'\\n'";
            else if (c == 13) cout << "'\\r'";
            else cout << "'" << char(c) << "' ";
            cout << "  " << codebook[c] << endl;
        }

    
    //------- encode_message ---------
    cout << endl << "About to call encode_message..." << endl;
    encode_message(raw, encoded, codebook);
    cout << "...call to encode_message complete" << endl;
    encoded.close();

    //------- attempt to decode new myzip file ---------
    cout << endl << "About to run decoder on new " << filename << ".myzip file..." << endl;
    InBitStream newest;
    newest.open(filename+".myzip");
    Node* tree{build_tree(newest)};
    if (tree != nullptr) {
        draw_tree(tree, "tree2.png");
        cout << "Tree, as read by decoded, should be shown in tree2.png," << endl;
        ofstream output("COPY_" + filename);
        decode_message(newest, output, tree);
        cout << "and a copy of the original message (after being encoded/decoded) should be in COPY_" << filename << endl;
    }
    
    return EXIT_SUCCESS;
}
