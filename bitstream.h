#pragma once

#include <fstream>
#include <string>

// Output Bit Stream
class OutBitStream {
 public:

  // Default constructor.  bitstream is unusable until 'opened'
  OutBitStream();

  // Destructor (automatically closes an open file)
  ~OutBitStream();

  // Associates bistream with given filename
  bool open(const std::string& filename);

  // Checks whether bitstream is currently open and valid
  bool is_open() const;

  // Writes the specified number of trailing bits of value
  void write(int value, int numbits=1);

  // Closes stream
  void close();

 private:
  static const long LIMIT = 5000000;

  std::ofstream file;
  int buffer;
  int bufsize;
  long byteswritten;

  void _clear();
  int _rawdump(int value, int numbits);

  // disallow clones
  OutBitStream(const OutBitStream& obs);
  OutBitStream& operator=(const OutBitStream& obs);
};


class InBitStream {
 public:
  InBitStream();
  ~InBitStream();

  // Associates bistream with given filename
  bool open(const std::string& filename);

  // Checks whether bitstream is currently open and valid
  bool is_open() const;

  // Read specified number of bits from file
  // (will be padded with trailing zeros if eof reached)
  int read(int numbits=1);  // numbits is at most eight.

  // Have we reached end of data
  bool eof() const;

  // Closes file
  void close();


 private:
  std::ifstream file;
  int buffer;
  int bufsize;
  
  void _clear();
  void _prefetch();

  // disallow clones
  InBitStream(const InBitStream& ibs);
  InBitStream& operator=(const InBitStream& ibs);
};
