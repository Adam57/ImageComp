#ifndef VAR_BITS_H
#define VAR_BITS_H

#include <cstdint>
#include <vector>

class VarBits{
public:
	int bit1;
	int bit2;
	VarBits(int b1, int b2);
	std::vector<unsigned char> compress(std::vector<unsigned int> numbers);
	std::vector<int> decompress(unsigned char input[], int input_size);
};

#endif