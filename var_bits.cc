#include "var_bits.h"
#include "bits_utils.h"
#include <iostream>
#include <string>
#include <bitset>

VarBits::VarBits(int b1, int b2){
	bit1 = b1;
	bit2 = b2;
}

std::vector<unsigned char> VarBits::compress(std::vector<unsigned int> numbers){
  BitsWriter bw;
	unsigned int divider = 1<<bit1;
	int total_bits = 0;
	for(uint i = 0; i < numbers.size(); i++){
        if(numbers[i] >= divider){
          total_bits = bit1 + bit2 + 1;
          bw.WriteBits(numbers[i], total_bits);
        }else{
          int var = (1 << bit1);
          var |= numbers[i];
          total_bits = bit1 + 1;
          bw.WriteBits(var, total_bits);
        }
	}
  bw.FlushBuffer();
  return bw.data;
}

std::vector<int> VarBits::decompress(unsigned char input[], int input_size){
    BitsReader br(input, input_size);
    ReturnType rt;
    int number = 0;
    std::vector<int> results;

    while(1){
      if(br.TestCurrentBit()){
        rt = br.GetBits(bit1 + 1);
        // std::string binary = std::bitset<16>(rt.number).to_string(); //to binary
        // std::cout << binary << "\n";
        number = rt.number & ~(1 << bit1);
      }else{
        rt = br.GetBits(bit1 + bit2 + 1);
        // std::string binary = std::bitset<16>(rt.number).to_string(); //to binary
        // std::cout << binary << "\n";
        number = rt.number;
      }
      if(rt.valid_number == false){
          break;
      }
      results.push_back(number);
    }

    return results;
}