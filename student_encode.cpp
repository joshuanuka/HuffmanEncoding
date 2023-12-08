#include "student_encode.h"
#include <fstream>
#include <iostream>    // allow cout for debugging
#include <queue>
#include <vector>
using namespace std;


// Computes vector of length 257 storing frequencies for Huffman encoding of given plaintext
vector<int> compute_frequencies(ifstream& plaintext) {
    vector<int> counts(257,0);
    char c;
    while (plaintext.get(c)){
      counts[c]++;
    }
    counts[256]=1; //EOM character occurs once
    return counts;
}

// Returns root of newly allocated Huffman tree for given frequencies
Node* build_tree(const vector<int>& freq) {
    priority_queue<pair<int,Node*>, vector<pair<int,Node*>>, greater<pair<int, Node*>>> pq;
    for (int i=0;i<freq.size();i++){
      if (freq[i]>0){
        pq.push(make_pair(freq[i],new Node(i)));
      }
    }
  while(pq.size()!=1){
    Node* left=pq.top().second;
    int left_freq=pq.top().first;
    pq.pop();
    Node* right=pq.top().second;
    int right_freq=pq.top().first;
    pq.pop();
    Node* parent=new Node(-1, left,right);
    pq.push(make_pair(left_freq+right_freq,parent));
  }
  if (pq.empty()){
    return nullptr;
  }
  else{
    return pq.top().second;
  }
}

// Encodes the given (sub)tree within the given bitstream
void encode_tree(OutBitStream& bitstream, Node* subtree) {
  if (subtree==nullptr){
    return;}
  
  if (subtree->left==nullptr && subtree->right==nullptr){
    bitstream.write(1);
    bitstream.write(subtree->code,9);
  }
  else{
    bitstream.write(0);
    encode_tree(bitstream,subtree->left);
    encode_tree(bitstream,subtree->right);
  }
}


// Stores the '0'/'1' string associated with every leaf in the given subtree,
// presuming given coding for the path to reach the subtree
void fill_codebook(vector<string>& codebook, Node* subtree, string& path){

  if (subtree==nullptr){
    return;
  }
  else if (subtree->left==nullptr && subtree->right==nullptr){
    codebook[subtree->code]=path;
  }
  else{
    string pathLeft = path + "0";
    fill_codebook(codebook,subtree->left,pathLeft);
    string pathRight = path + "1";
    fill_codebook(codebook,subtree->right,pathRight);
  }
}

// Inserts the message encoding for plaintext into the bitstream using the codebook
void encode_message(ifstream& plaintext, OutBitStream& bitstream, const vector<string>& codebook) {
  char c;
  while (plaintext.get(c)){
    const string& code = codebook[c];
    for (char bit: code){
      if (bit=='1'){
      bitstream.write(1);
      }
      else{
        bitstream.write(0);
      }
    }
  }
  //EOM character
  const string& EOMcode = codebook[256];
  for (char bit: EOMcode){
    if (bit=='1'){
    bitstream.write(1);
    }
    else{
      bitstream.write(0);
    }
  }
}

