#ifndef BITS_UTILS_H
#define BITS_UTILS_H

#include <cstdint>
#include <vector>
#include <string>
#include <assert.h>

struct ReturnType{
	bool valid_number;
	int number;// the returned bits is between 1~32, stored in the lower bits of this int
};

class BitsWriter{
public:
	std::vector<unsigned char> data;
	uint16_t offset, //track the bit within a byte
	size, //how many bytes we have
	data_unit_size; //track the size of each number
	uint32_t tmp;

	BitsWriter();
	void WriteBits(int number, int number_of_bits);
	void PushBack();
	void FlushBuffer();
	void WriteToDisk(const std::string& path);
	void ClearBuffer();

	void DisplayData();
	void test();
};

class BitsReader{
public:
	unsigned char* data;
	uint16_t offset, //track the bit within a byte
	index, //track the current byte
	size, //how many bytes we have
	data_unit_size, //track the size of each number
	first_part, //store the first part of the data, if the data is across multiple chars
	last_part,
	bits_first_part,//store how many bits the first part has
	bits_last_part,
	mid_bytes; //store how many full bytes the data covers except for the part in the beginning and end

	BitsReader(unsigned char input[], int input_size);
	bool TestCurrentBit();
	ReturnType GetBits(int number_of_bits); //read bits and update offset
	ReturnType ReadBits(int number_of_bits); //read bits but don't update offset
	int GetNumberOfRemainingBits();
	void test();

};

#endif