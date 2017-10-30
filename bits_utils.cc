#include "bits_utils.h"
#include <iostream>
#include <string>
#include <bitset>


void SetBit( int A[],  int k ){
      A[k/32] |= 1 << (k%32);  // Set the bit at the k-th position in A[i]
}

void ClearBit( int A[],  int k ){
      A[k/32] &= ~(1 << (k%32));
}

int TestBit( int A[],  int k ){
      return ( (A[k/32] & (1 << (k%32) )) != 0 );     
}

/*For BitsWriter*/
BitsWriter::BitsWriter(){
	// std::cout << "BitsWriter Constructor Called\n";
	data.clear();
	offset = 0; //track the bit within a byte
	data_unit_size = sizeof(uint32_t) * 8; //track the size of each number
	tmp = 0; //a 4 byte buffer
}

void BitsWriter::WriteToDisk(const std::string& path){

	// FILE *fdo = fopen(path.c_str(),"w");
	FILE *fdo = fopen(path.c_str(),"a");
  	if(! fdo) {
    	std::cout << "Failed writing to " << path << " (Hint: create folder for path?)" << std::endl;
    	return;
  	}
  	assert(fdo);
  	uint16_t size = data.size();
  	std::cout << "data size: "<< size << "\n";
  	/*write data size first 16 bits first*/
  	if(fwrite(&size, sizeof(uint16_t), 1, fdo)!=1){
  		std::cout << "fail to write data to file: " << path << "\n";
  	}

  	/*write the content second*/
  	for(uint i = 0; i < data.size(); i++){
  		if(fwrite(&data[i], sizeof(unsigned char), 1, fdo)!=1){
  			std::cout << "fail to write data to file: " << path << "\n";
  		}
  	}
  	fclose(fdo);
}

void BitsWriter::ClearBuffer(){
	tmp = 0;
	offset = 0;
	data.clear();
}

void BitsWriter::test(){
	// WriteBits(7, 4);
	// WriteBits(1872, 11);
	// WriteBits(1772, 11);
	// WriteBits(1578, 11);
	// WriteBits(125, 7);
	// WriteBits(0, 0);
	// WriteBits(124522, 6);
	WriteBits(1, 21);
	WriteBits(3, 21);
	FlushBuffer();
	DisplayData();
}

void BitsWriter::DisplayData(){
  for(int i = 0; i < data.size(); i++){
    std::string binary = std::bitset<8>(data[i]).to_string(); //to binary
    std::cout << "char: " << (int)data[i] << " in binary format: " << binary << "\n";
  } 
}

void BitsWriter::WriteBits(int number, int number_of_bits){
	if( number_of_bits == 0){
		return;
	}

	if(number_of_bits > 32){
		std::cout << "do not support bits more than 32 at this point..";
		return;
	}

	// std::string binary = std::bitset<16>(number).to_string(); //to binary
 //    std::cout << number_of_bits << " bits for int: " << number << " in binary format: " << binary << "\n";
	int total_bits = offset + number_of_bits;
	if( total_bits <= data_unit_size ){//adding this number does not need another int
		number = number << (data_unit_size - total_bits);
		tmp |= number;

		/*update the offset*/
		offset += number_of_bits;

		// binary = std::bitset<32>(tmp).to_string(); //to binary
  //   	std::cout << "int: " << tmp << " in binary format: " << binary << "\n";

  //   	std::cout << "current offset: " << offset << "\n";
	}else{//adding this number need another int
		/*deal with the first part*/
		int bits_in_first_int = data_unit_size - offset;
		int bits_in_second_int = number_of_bits - bits_in_first_int;
		uint32_t first_part = number >> bits_in_second_int;
		tmp |= first_part;
		// binary = std::bitset<32>(tmp).to_string(); //to binary
  //   	std::cout << "int: " << tmp << " in binary format: " << binary << "\n";
		PushBack();
		/*deal with the second part*/
		uint32_t second_part = number << (data_unit_size - bits_in_second_int);
		tmp = 0;//clear the buffer
		tmp |= second_part;

		/*update the offset*/
		offset += number_of_bits;
		offset %= data_unit_size;

		// binary = std::bitset<32>(tmp).to_string(); //to binary
    	// std::cout << "int: " << tmp << " in binary format: " << binary << "\n";

    	// std::cout << "current offset: " << offset << "\n";
    	// FlushBuffer();
	}
}

void BitsWriter::PushBack(){
	  unsigned char tmp_char = (tmp>>24)  & 0xff;//take first 8 bits
	  data.push_back(tmp_char);
	  tmp_char = (tmp>>16)  & 0xff;//take second 8 bits
	  data.push_back(tmp_char);
      tmp_char = (tmp>>8)  & 0xff;//take third 8 bits
      data.push_back(tmp_char);
      tmp_char = tmp & 0xff; //take forth 8 bits
      data.push_back(tmp_char);
}

void BitsWriter::FlushBuffer(){
	  unsigned char tmp_char;
	  if( offset > 24 ){
	  	  tmp_char = (tmp>>24)  & 0xff;//take first 8 bits
		  data.push_back(tmp_char);
		  tmp_char = (tmp>>16)  & 0xff;//take second 8 bits
		  data.push_back(tmp_char);
		  tmp_char = (tmp>>8)  & 0xff;//take third 8 bits
		  data.push_back(tmp_char);
		  tmp_char = tmp & 0xff; //take forth 8 bits
      	  data.push_back(tmp_char);
      }
	  else if( offset > 16 && offset <= 24){
	  	  tmp_char = (tmp>>24)  & 0xff;//take first 8 bits
		  data.push_back(tmp_char);
		  tmp_char = (tmp>>16)  & 0xff;//take second 8 bits
		  data.push_back(tmp_char);
		  tmp_char = (tmp>>8)  & 0xff;//take third 8 bits
		  data.push_back(tmp_char);
      }
      else if( offset > 8 && offset <= 16 ){
	  	  tmp_char = (tmp>>24)  & 0xff;//take first 8 bits
		  data.push_back(tmp_char);
		  tmp_char = (tmp>>16)  & 0xff;//take second 8 bits
		  data.push_back(tmp_char);
      }else{
	  	  tmp_char = (tmp>>24)  & 0xff;//take first 8 bits
		  data.push_back(tmp_char);
      }	
}

/*For BitsReader*/
BitsReader::BitsReader(unsigned char input[], int input_size){
	data = input;
	size = input_size;
	offset = 0; //track the bit within a byte
	index = 0; //track the current byte
	data_unit_size = sizeof(char) * 8; //track the size of each number
	first_part = 0; //store the first part of the data, if the data is across multiple chars
	last_part = 0;
	bits_first_part = 0;//store how many bits the first part has
	bits_last_part = 0;
	mid_bytes = 0; //store how many full bytes the data covers except for the part in the beginning and end

	// std::cout << size << "\n";
}

void BitsReader::test(){
	int number = GetBits(21).number;
	std::string binary = std::bitset<21>(number).to_string(); //to binary
    std::cout << "int: " << number << " in binary format: " << binary << "\n";
    // std::cout << TestCurrentBit() << "\n";
    number = GetBits(21).number;
    binary = std::bitset<21>(number).to_string(); //to binary
    std::cout << "int: " << number << " in binary format: " << binary << "\n";
    // number = GetBits(27).number;
    // binary = std::bitset<27>(number).to_string(); //to binary
    // std::cout << "int: " << number << " in binary format: " << binary << "\n";
    // number = GetBits(4).number;
    // binary = std::bitset<4>(number).to_string(); //to binary
    // std::cout << "int: " << number << " in binary format: " << binary << "\n";
}

bool BitsReader::TestCurrentBit(){
	return ( (data[index] & (1 << (data_unit_size - offset - 1) )) != 0 );  //offset is 0 ~ 7
}

int BitsReader::GetNumberOfRemainingBits(){
	return ( (size - index - 1) * 8 + (data_unit_size - offset) );
}

ReturnType BitsReader::GetBits(int number_of_bits){
	ReturnType rt;

	// if(number_of_bits == 0){
	// 	std::cout << "no bits is read\n";
	// 	rt.valid_number = false;
	// 	return rt;
	// }

	if(number_of_bits == 0){
		// std::cout << "no bits is read\n";
		rt.valid_number = true;
		rt.number = 0;
		return rt;
	}

	if(number_of_bits >= 32){
		std::cout << "does not support bits larger or equal to 32\n";
		rt.valid_number = false;
		return rt;
	}

	if( GetNumberOfRemainingBits() < number_of_bits){
		// std::cout << GetNumberOfRemainingBits() << " " << number_of_bits << "\n";
		// std::cout << "data reading out of boudary..\n";
		rt.valid_number = false;
		return rt;
	}

	int total_bits = offset + number_of_bits;
	/*when the bits we need are all in one char*/
	if(total_bits <= data_unit_size){
		rt.number = data[index];
		/*clear the bits before offset*/
		rt.number &= (1 << (data_unit_size - offset)) - 1;
		/*take the needed bits*/
		rt.number = rt.number >> (data_unit_size - total_bits);
		rt.valid_number =  true;

		/*update offset*/
		offset += number_of_bits;
		offset %= data_unit_size;
		if(offset == 0){// don't forget this corner case, if a byte is fully read here, increase the index
			index ++;
		}
		// std::cout << "current offset: " << offset << "\n";
		return rt;
	}
	if(total_bits > data_unit_size){
		/*fetch the data in the first byte*/
		bits_first_part = data_unit_size - offset;
		rt.number = data[index];
		rt.number &= (1 << bits_first_part) - 1; //erase the unrelated bits
		index ++;

		/*if the data covers some whole bytes*/
		mid_bytes = (total_bits - data_unit_size)/data_unit_size;
		if(mid_bytes != 0){
			rt.number = rt.number << (mid_bytes * 8); //make space for the bits in whole bytes
		}
		for(uint i = 0; i < mid_bytes; i++){
			rt.number |= (data[index + i] << ((mid_bytes - i - 1) * 8) );
		}
		index += mid_bytes;

		/*if the data still have the third part remaining*/
		bits_last_part = (total_bits - data_unit_size)%8;
		if(bits_last_part!= 0){
			rt.number = rt.number << (bits_last_part); //make space for the third part
			rt.number |= (data[index] >> (data_unit_size - bits_last_part)); //adding the last part
		}

		/*update offset*/
		offset += number_of_bits;
		offset %= data_unit_size;
		// std::cout << "current offset: " << offset << "\n";
		rt.valid_number =  true;
		return rt;
	}

	/*something is wrong, if it reaches here*/
	rt.valid_number = false;
	return rt;
}

ReturnType BitsReader::ReadBits(int number_of_bits){
	ReturnType rt;

	uint16_t origin_offset = offset; //track the bit within a byte
	uint16_t origin_index = index; //track the current byte

	if(number_of_bits == 0){
		std::cout << "no bits is read\n";
		rt.valid_number = false;
		return rt;
	}

	if(number_of_bits >= 32){
		std::cout << "does not support bits larger or equal to 32\n";
		rt.valid_number = false;
		return rt;
	}

	if( GetNumberOfRemainingBits() < number_of_bits){
		// std::cout << GetNumberOfRemainingBits() << " " << number_of_bits << "\n";
		// std::cout << "data reading out of boudary..\n";
		rt.valid_number = false;
		return rt;
	}

	int total_bits = offset + number_of_bits;
	/*when the bits we need are all in one char*/
	if(total_bits <= data_unit_size){
		rt.number = data[index];
		/*clear the bits before offset*/
		rt.number &= (1 << (data_unit_size - offset)) - 1;
		/*take the needed bits*/
		rt.number = rt.number >> (data_unit_size - total_bits);
		rt.valid_number =  true;

		/*update offset*/
		offset += number_of_bits;
		offset %= data_unit_size;
		if(offset == 0){// don't forget this corner case, if a byte is fully read here, increase the index
			index ++;
		}
		// std::cout << "current offset: " << offset << "\n";

		/*reset the offset and index*/
		offset = origin_offset;
		index = origin_index;
		return rt;
	}
	if(total_bits > data_unit_size){
		/*fetch the data in the first byte*/
		bits_first_part = data_unit_size - offset;
		rt.number = data[index];
		rt.number &= (1 << bits_first_part) - 1; //erase the unrelated bits
		index ++;

		/*if the data covers some whole bytes*/
		mid_bytes = (total_bits - data_unit_size)/data_unit_size;
		if(mid_bytes != 0){
			rt.number = rt.number << (mid_bytes * 8); //make space for the bits in whole bytes
		}
		for(uint i = 0; i < mid_bytes; i++){
			rt.number |= (data[index + i] << ((mid_bytes - i - 1) * 8) );
		}
		index += mid_bytes;

		/*if the data still have the third part remaining*/
		bits_last_part = (total_bits - data_unit_size)%8;
		if(bits_last_part!= 0){
			rt.number = rt.number << (bits_last_part); //make space for the third part
			rt.number |= (data[index] >> (data_unit_size - bits_last_part)); //adding the last part
		}

		/*update offset*/
		offset += number_of_bits;
		offset %= data_unit_size;
		// std::cout << "current offset: " << offset << "\n";

		/*reset the offset and index*/
		offset = origin_offset;
		index = origin_index;
		rt.valid_number =  true;
		return rt;
	}

	/*something is wrong, if it reaches here*/
	/*reset the offset and index*/
	offset = origin_offset;
	index = origin_index;
	rt.valid_number = false;
	return rt;
}