/*************************************************************************
* Name:        compressor.h
* Author:      Qi Wang
* Description: coder/decoder interface.
* Copyright:   2017 Blippar
*-------------------------------------------------------------------------
*************************************************************************/

#include "compressor.h"
#include <netinet/in.h>
#include <memory>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <bitset>
#include <vector>
#include "huffman.h"
#include "move_to_front.h"
#include "bits_utils.h"


bool SortByXYSCALE(KeypointEntry i, KeypointEntry j){
  if(i.x < j.x){
    return true;
  }else if(i.x > j.x){
    return false;
  }else{
    if(i.y < j.y){
      return true;
    }else if(i.y > j.y){
      return false;
    }else{
      if(i.fsize < j.fsize){
        return true;
      }else{
        return false;
      }
    }
  }
}

bool UniqueByXYScale (KeypointEntry i, KeypointEntry j) {
  return ( (i.x == j.x) && (i.y == j.y) && (i.fsize == j.fsize) );
}

bool SortByLengthThenSymbolHuffmanNode(CanonicalHuffmanNode a, CanonicalHuffmanNode b){
  if ( a.length > b.length ){
    return true;
  }else if( a.length < b.length){
    return false;
  }else{
    if(a.symbol < b.symbol){
      return true;
    }else{
      return false;
    }
  }
}

bool SortByLengthThenSymbolHuffmanNodeLess(CanonicalHuffmanNode a, CanonicalHuffmanNode b){
  if ( a.length < b.length ){
    return true;
  }else if( a.length > b.length){
    return false;
  }else{
    if(a.symbol < b.symbol){
      return true;
    }else{
      return false;
    }
  }
}

bool SortBySymbolHuffmanNode(CanonicalHuffmanNode a, CanonicalHuffmanNode b){
	if(a.symbol < b.symbol){
		return true;
	}else{
		return false;
	}
}

bool Compressor::equalsOnXYScale(KeypointEntry k1, KeypointEntry k2){
	if( (k1.x == k2.x) && (k1.y == k2.y) && (k1.fsize == k2.fsize) ){
		// std::cout << "equals " << k1.x << " " << k2.x << " " << k1.y << " " << k2.y << " " << k1.fsize << " " << k2.fsize << "\n";
		return true;
	}else{
		// std::cout << "not equals " << k1.x << " " << k2.x << " " << k1.y << " " << k2.y << " " << k1.fsize << " " << k2.fsize << "\n";
		return false;
	}
}

void GetCanonicalHuffmanCode(std::vector<CanonicalHuffmanNode>& huffman_code_vector, int huffman_bits_per_bin[], int size){
    /*build the huffman node table*/
    for(int i = 0; i < size; i++){
      CanonicalHuffmanNode node(i, huffman_bits_per_bin[i]);//symbol is the index of bin
      huffman_code_vector.push_back(node);
    }
    /*sort by length*/
    std::sort(huffman_code_vector.begin(), huffman_code_vector.end(), SortByLengthThenSymbolHuffmanNode);

    // for(int i = 0; i < huffman_code_vector.size(); i++){
    //   std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << "\n";
    // }

    /*init the longest length and value*/
    int length = huffman_code_vector[0].length;
    huffman_code_vector[0].value = 0;

    /*set every value*/
    for(int i = 1; i < huffman_code_vector.size(); i++){
        if(huffman_code_vector[i].length == 0){
          break;
        }
        if(huffman_code_vector[i].length == length){
          huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
        }
        if(huffman_code_vector[i].length < length){
          huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
          int diff = length - huffman_code_vector[i].length; //if length go from 4 to 2, need to shift 2 bits to left
          length = huffman_code_vector[i].length;
          while(diff > 0){
             huffman_code_vector[i].value = huffman_code_vector[i].value  >> 1;
             diff --;
          }
        }
    }  

    /*sort by symbol again to write header info*/
	std::sort(huffman_code_vector.begin(), huffman_code_vector.end(), SortBySymbolHuffmanNode);
	/*output all the symbols*/  
	const long unsigned int bit_number = 8;
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::string binary = std::bitset<bit_number>(huffman_code_vector[i].value).to_string(); //to binary
      std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << " binary: " << binary << "\n";
    }
}

void GetCanonicalHuffmanCodeDecompress(std::vector<CanonicalHuffmanNode>& huffman_code_vector, int huffman_bits_per_bin[], int size){
    /*build the huffman node table*/
    for(int i = 0; i < size; i++){
      if(huffman_bits_per_bin[i] != 0 ){
      	CanonicalHuffmanNode node(i, huffman_bits_per_bin[i]);//symbol is the index of bin
      	huffman_code_vector.push_back(node);
      }
    }
    /*sort by length*/
    std::sort(huffman_code_vector.begin(), huffman_code_vector.end(), SortByLengthThenSymbolHuffmanNode);

    // for(int i = 0; i < huffman_code_vector.size(); i++){
    //   std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << "\n";
    // }

    /*init the longest length and value*/
    int length = huffman_code_vector[0].length;
    huffman_code_vector[0].value = 0;

    /*set every value*/
    for(int i = 1; i < huffman_code_vector.size(); i++){
        if(huffman_code_vector[i].length == 0){
          break;
        }
        if(huffman_code_vector[i].length == length){
          huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
        }
        if(huffman_code_vector[i].length < length){
           huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
          int diff = length - huffman_code_vector[i].length; //if length go from 4 to 2, need to shift 2 bits to left
          length = huffman_code_vector[i].length;
          while(diff > 0){
             huffman_code_vector[i].value = huffman_code_vector[i].value  >> 1;
             diff --;
          }
        }
    }  

    /*sort by symbol again to write header info*/
	std::sort(huffman_code_vector.begin(), huffman_code_vector.end(), SortByLengthThenSymbolHuffmanNodeLess);
	/*display all the symbols*/  
	const long unsigned int bit_number = 8;
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::string binary = std::bitset<bit_number>(huffman_code_vector[i].value).to_string(); //to binary
      std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << " binary: " << binary << "\n";
    }
}

void Compressor::compress(){
	LoadBinaryDataFromFile();
	compressAttributes();

}

void Decompressor::decompressX(std::vector<int>& x_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader){
    XHuffmanInfoEntry x_info;
    //for x read 6 bins
    int* huffman_bits_per_bin = new int[X_SIZE];
    for(int i = 0; i < X_SIZE; i++){
      huffman_bits_per_bin[i] = dataReader.GetBits(X_BITS_IN_HEADER).number;
      std::cout << huffman_bits_per_bin[i] << " ";
    }
    std::cout << "\n";
    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCodeDecompress(huffman_code_vector, huffman_bits_per_bin, X_SIZE);

    /*read huffman data*/
    int minbits = huffman_code_vector[0].length;
    int maxbits = huffman_code_vector[huffman_code_vector.size() - 1].length;
    std::cout << minbits << " " << maxbits << "\n";

    int symbol = 0;
    int bits_for_number = 0;
    int number = 0;
    int counter = 0;
    for(int i = 0; i < image_unique_length; i++){
      //check the length of write 
      for(int j = 0; j < huffman_code_vector.size(); j++){
        int value = dataReader.ReadBits(huffman_code_vector[j].length).number; //read but don't move the pointer
        if(value == huffman_code_vector[j].value){
          dataReader.GetBits(huffman_code_vector[j].length); //move the pointer
          symbol = huffman_code_vector[j].symbol;
          bits_for_number = x_info.bits_per_bin[symbol];
          break;
        }
      }
      bits_for_number = x_info.bits_per_bin[symbol];
      number = dataReader.GetBits(bits_for_number).number;
      
      /*the last bin covers range [16, 512), it's not a range like[2^k-1, 2^k], so can't minus boundary to save 1 bit*/
      if(number < x_info.boundary[X_SIZE - 1]){
        number += x_info.boundary[symbol];
      }

      /*recover delta*/
      if(i != 0){
        number += x_vector[counter - 1];
      }
      x_vector.push_back(number);

      /*recover duplicates*/
      counter++;
      while( (bit_vector[counter] == 0) && (counter < bit_vector.size()) ){
        x_vector.push_back(number);
        counter++;
      }

    }

    delete[] huffman_bits_per_bin;
    // for(int i = 0; i < x_vector.size(); i++){
    //   std::cout << x_vector[i] << " ";
    // }
    // std::cout << "\n";  
}

void Decompressor::decompressY(std::vector<int>& y_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader){
    YHuffmanInfoEntry y_info;
    //for x read 6 bins
    int* huffman_bits_per_bin = new int[Y_SIZE];
    for(int i = 0; i < Y_SIZE; i++){
      huffman_bits_per_bin[i] = dataReader.GetBits(Y_BITS_IN_HEADER).number;
      std::cout << huffman_bits_per_bin[i] << " ";
    }
    std::cout << "\n";
    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCodeDecompress(huffman_code_vector, huffman_bits_per_bin, Y_SIZE);

    /*read huffman data*/
    int minbits = huffman_code_vector[0].length;
    int maxbits = huffman_code_vector[huffman_code_vector.size() - 1].length;
    std::cout << minbits << " " << maxbits << "\n";

    int symbol = 0;
    int bits_for_number = 0;
    int number = 0;
    int counter = 0;
    for(int i = 0; i < image_unique_length; i++){
      //check the length of write 
      for(int j = 0; j < huffman_code_vector.size(); j++){
        int value = dataReader.ReadBits(huffman_code_vector[j].length).number; //read but don't move the pointer
        if(value == huffman_code_vector[j].value){
          dataReader.GetBits(huffman_code_vector[j].length); //move the pointer
          symbol = huffman_code_vector[j].symbol;
          bits_for_number = y_info.bits_per_bin[symbol];
          break;
        }
      }
      bits_for_number = y_info.bits_per_bin[symbol];
      number = dataReader.GetBits(bits_for_number).number;
      
      number += y_info.boundary[symbol];
      
      y_vector.push_back(number);

      /*recover duplicates*/
      counter++;
      while( (bit_vector[counter] == 0) && (counter < bit_vector.size()) ){
        y_vector.push_back(number);
        counter++;
      }

    }

    delete[] huffman_bits_per_bin;
}

void Decompressor::decompressScale(std::vector<int>& scale_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader){
    ScaleHuffmanInfoEntry scale_info;

    /*read min num*/
    scale_info.min = dataReader.GetBits(16).number;
    std::cout << "min number: " << scale_info.min << "\n";

    //read header info
    int* huffman_bits_per_bin = new int[SCALE_SIZE];
    for(int i = 0; i < SCALE_SIZE; i++){
      huffman_bits_per_bin[i] = dataReader.GetBits(SCALE_BITS_IN_HEADER).number;
      std::cout << huffman_bits_per_bin[i] << " ";
    }
    std::cout << "\n";
    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCodeDecompress(huffman_code_vector, huffman_bits_per_bin, SCALE_SIZE);

    /*read huffman data*/
    int minbits = huffman_code_vector[0].length;
    int maxbits = huffman_code_vector[huffman_code_vector.size() - 1].length;
    std::cout << minbits << " " << maxbits << "\n";

    int symbol = 0;
    int bits_for_number = 0;
    int number = 0;
    int counter = 0;
    int value = 0;
    int i, j;
    for(i = 0; i < image_unique_length; i++){
      //check the length of write 
      for(j = 0; j < huffman_code_vector.size(); j++){
        value = dataReader.ReadBits(huffman_code_vector[j].length).number; //read but don't move the pointer
        if(value == huffman_code_vector[j].value){
          // std::cout << "hit!\nvalue: " << value << " length: " << huffman_code_vector[j].length << " j: " << j << "\n";
          dataReader.GetBits(huffman_code_vector[j].length); //move the pointer
          symbol = huffman_code_vector[j].symbol;
          bits_for_number = scale_info.bits_per_bin[symbol];
          break;
        }
      }
      bits_for_number = scale_info.bits_per_bin[symbol];
      number = dataReader.GetBits(bits_for_number).number;
      // std::cout << "value: " << value << " length: " << huffman_code_vector[j].length <<" number without boundary: " << number <<" symbol: " << symbol << " boundary: " << scale_info.boundary[symbol] <<" bits_for_number: " << bits_for_number<< "\n";
      number += scale_info.boundary[symbol];

      number += scale_info.min;
      
      scale_vector.push_back(number);

      /*recover duplicates*/
      counter++;
      while( (bit_vector[counter] == 0) && (counter < bit_vector.size()) ){
        scale_vector.push_back(number);
        counter++;
      }

    }

    delete[] huffman_bits_per_bin;  
}

void Decompressor::decompressAngle(std::vector<int>& angle_vector, int image_length, BitsReader& dataReader){
  for(int i = 0; i < image_length; i++){
    angle_vector.push_back(dataReader.GetBits(ANGLE_MAX_BITS).number);
  }
}

void Decompressor::decompressCW(std::vector<int>& cw_vector, int image_length, BitsReader& dataReader){
  for(int i = 0; i < image_length; i++){
    cw_vector.push_back(dataReader.GetBits(CW_MAX_BITS).number);
  }
}

void Decompressor::decompress(){
	/*load the compressed data*/
	FILE* file = fopen(this->compressedFile.c_str(),"r");
  	if( file == NULL) {
    	std::cout << "Problem! The file: " << this->compressedFile << " could not be opened!\n";
    	exit(0);
  	}  	

  	int size = 0;
  	while(fread(&size, 1, 2, file) == 2){// 16bits indicates the size of the following document

		/*read current document into memory*/
		std::cout << "data size: " << size << "\n";
		unsigned char*  input = new unsigned char[size];
    fread(input, 1, size, file);
    // BitsReader* dataReader = new BitsReader(input, size);
    BitsReader dataReader(input, size);

	  /*read bit stream*/
		int image_length = dataReader.GetBits(11).number;
    int image_unique_length = 0;
    std::vector<int> bit_vector; //recover the duplicates
		for(int i = 0; i < image_length; i++){
			int flag_bit = dataReader.GetBits(1).number;
      bit_vector.push_back(flag_bit);
      if(flag_bit == 1){
        image_unique_length ++;
      }
			std::cout << flag_bit;
		}
		std::cout << "\nimage length: " << image_length << "\n";

    /*decompress angle*/
    std::vector<int> angle_vector;
    decompressAngle(angle_vector, image_length, dataReader);
    for(int i = 0; i < angle_vector.size(); i++){
      std::cout << angle_vector[i] << " ";
    }
    std::cout << "\n"; 

    /*decompress cw*/
    std::vector<int> cw_vector;
    decompressCW(cw_vector, image_length, dataReader);
    for(int i = 0; i < cw_vector.size(); i++){
      std::cout << cw_vector[i] << " ";
    }
    std::cout << "\n"; 

    /*decompress x*/
    std::vector<int> x_vector;
    decompressX(x_vector, bit_vector, image_unique_length, dataReader);
    for(int i = 0; i < x_vector.size(); i++){
      std::cout << x_vector[i] << " ";
    }
    std::cout << "\n"; 

    /*decompress y*/
    std::vector<int> y_vector;
    decompressY(y_vector, bit_vector, image_unique_length, dataReader);
    for(int i = 0; i < y_vector.size(); i++){
      std::cout << y_vector[i] << " ";
    }
    std::cout << "\n"; 

    /*decompress scale*/
    std::vector<int> scale_vector;
    decompressScale(scale_vector, bit_vector, image_unique_length, dataReader);
    for(int i = 0; i < scale_vector.size(); i++){
      std::cout << scale_vector[i] << " ";
    }
    std::cout << "\n"; 

		delete[] input;
		// delete dataReader;
	}
	fclose(file);
}

void Compressor::compressX(std::vector<KeypointEntry> kps){
  /*process x*/
    /*take delta for x*/
    for(uint i = kps.size() - 1; i >= 1; i--){
          kps[i].x = kps[i].x - kps[i-1].x;
    }

    /*display data*/
    for(uint i = 0; i < kps.size(); i++){
      std::cout << kps[i].x << " ";
    }
    std::cout << "\n";

    /*fill up huffman bins*/
    XHuffmanInfoEntry x_info;
    for(uint i = 0; i < kps.size(); i++){
      for(int j = 1; j < x_info.number_of_boundries; j++){
        if( (kps[i].x >= x_info.boundary[j - 1]) && (kps[i].x < x_info.boundary[j]))
          x_info.bins[j - 1] += 1;
      }
    }

    /*get huffman code length*/
    Huffman_Canonical(x_info.bins, x_info.number_of_boundries - 1, x_info.huffman_bits_per_bin);

    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCode(huffman_code_vector, x_info.huffman_bits_per_bin, x_info.number_of_boundries - 1);

    /*write huffman header info*/
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::cout << "writing bin length value: " << huffman_code_vector[i].length << " length: " << X_BITS_IN_HEADER << "\n";
      dataWriter.WriteBits(huffman_code_vector[i].length, X_BITS_IN_HEADER);//6 bins for x, so the max length can only be 6, 3 bits are enough
    }

    /*write huffman code + data*/
    for(uint i = 0; i < kps.size(); i++){
      int symbol = 0;
      int value = 0;
      int code_length = 0;
      int num_length = 0;
      for(int j = 1; j < x_info.number_of_boundries; j++){
        // std::cout << "number: " << kps[i].x  << " " << x_info.boundary[j - 1] << " " << x_info.boundary[j] << "\n";
        if( (kps[i].x >= x_info.boundary[j - 1]) && (kps[i].x < x_info.boundary[j])){
          symbol = j - 1;
        }else{
          continue;
        }
        // std::cout << "symbol: " << symbol << "\n";
        value = huffman_code_vector[symbol].value;
        code_length = huffman_code_vector[symbol].length;
        num_length = x_info.bits_per_bin[symbol];
        // std::cout << "num_length: " << num_length << "\n";
        /*write huffman prefix*/
        // std::cout << "writing huffman code prefix: " << value << " length: " << code_length << "\n";
        dataWriter.WriteBits(value, code_length);
        /*write number itself*/
        // std::cout << "writing number: " << kps[i].x - x_info.boundary[symbol] << " length: " << num_length << "\n";
        /*the last bin covers range [16, 512), it's not a range like[2^k-1, 2^k], so can't minus boundary to save 1 bit*/
        if( kps[i].x < x_info.boundary[X_SIZE - 1]){
           dataWriter.WriteBits(kps[i].x - x_info.boundary[symbol], num_length);
        }else{
           dataWriter.WriteBits(kps[i].x, num_length);
        }
      }
    }
    // /*flush the data*/
    // dataWriter.FlushBuffer();
    // // dataWriter.DisplayData();
    // const std::string path = "File718";
    // dataWriter.WriteToDisk(path);
    // dataWriter.ClearBuffer();
}

void Compressor::compressY(std::vector<KeypointEntry> kps){

    /*display y*/
    for(uint i = 0; i < kps.size(); i++){
      std::cout << kps[i].y << " ";
    }
    std::cout << "\n";

    /*fill up huffman bins*/
    YHuffmanInfoEntry y_info;
    for(uint i = 0; i < kps.size(); i++){
      for(int j = 1; j < y_info.number_of_boundries; j++){
        if( (kps[i].y >= y_info.boundary[j - 1]) && (kps[i].y < y_info.boundary[j]))
          y_info.bins[j - 1] += 1;
      }
    }

    /*get huffman code length*/
    Huffman_Canonical(y_info.bins, y_info.number_of_boundries - 1, y_info.huffman_bits_per_bin);

    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCode(huffman_code_vector, y_info.huffman_bits_per_bin, y_info.number_of_boundries - 1);

    /*write huffman header info*/
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::cout << "writing bin length value: " << huffman_code_vector[i].length << " length: " << Y_BITS_IN_HEADER << "\n";
      dataWriter.WriteBits(huffman_code_vector[i].length, Y_BITS_IN_HEADER);//5 bins for y, so the max length can only be 5, 3 bits are enough
    }

    /*write huffman code + data*/
    for(uint i = 0; i < kps.size(); i++){
      int symbol = 0;
      int value = 0;
      int code_length = 0;
      int num_length = 0;
      for(int j = 1; j < y_info.number_of_boundries; j++){
        // std::cout << "number: " << kps[i].x  << " " << x_info.boundary[j - 1] << " " << x_info.boundary[j] << "\n";
        if( (kps[i].y >= y_info.boundary[j - 1]) && (kps[i].y < y_info.boundary[j])){
          symbol = j - 1;
        }else{
          continue;
        }
        // std::cout << "symbol: " << symbol << "\n";
        value = huffman_code_vector[symbol].value;
        code_length = huffman_code_vector[symbol].length;
        num_length = y_info.bits_per_bin[symbol];
        // std::cout << "num_length: " << num_length << "\n";
        /*write huffman prefix*/
        // std::cout << "writing huffman code prefix: " << value << " length: " << code_length << "\n";
        dataWriter.WriteBits(value, code_length);
        /*write number itself*/
        // std::cout << "writing number: " << kps[i].y - y_info.boundary[symbol] << " length: " << num_length << "\n";
        dataWriter.WriteBits(kps[i].y - y_info.boundary[symbol], num_length);
      }
    }
    // /*flush the data*/
    // dataWriter.FlushBuffer();
    // // dataWriter.DisplayData();
    // const std::string path = "File718";
    // dataWriter.WriteToDisk(path);
    // dataWriter.ClearBuffer();
}

void Compressor::compressScale(std::vector<KeypointEntry> kps){
    ScaleHuffmanInfoEntry scale_info;
    /*get the minimum value*/
    scale_info.min = kps[0].fsize;
    for(uint i = 0; i < kps.size(); i++){
      if(scale_info.min > kps[i].fsize){
        scale_info.min = kps[i].fsize;
      }
    }
    // std::cout << "min value: " << scale_info.min << "\n";
    /*to be safe use 16 bits to store the minimum, even tho less bits could be used here*/
    dataWriter.WriteBits(scale_info.min, 16);

    for(uint i = 0; i < kps.size(); i++){
      kps[i].fsize -= scale_info.min;
    }

    /*fill up huffman bins*/
    for(uint i = 0; i < kps.size(); i++){
      // kps[i].fsize -= scale_info.min; // every thing deduct the minimum
      for(int j = 1; j < scale_info.number_of_boundries; j++){
        if( (kps[i].fsize >= scale_info.boundary[j - 1]) && (kps[i].fsize < scale_info.boundary[j]))
          scale_info.bins[j - 1] += 1;
      }
    }

    /*display scale*/
    for(uint i = 0; i < kps.size(); i++){
      std::cout << kps[i].fsize << " ";
    }
    std::cout << "\n";

    /*get huffman code length*/
    Huffman_Canonical(scale_info.bins, scale_info.number_of_boundries - 1, scale_info.huffman_bits_per_bin);

    /*get the actual value of canonical huffman codes*/
    std::vector<CanonicalHuffmanNode> huffman_code_vector;
    GetCanonicalHuffmanCode(huffman_code_vector, scale_info.huffman_bits_per_bin, scale_info.number_of_boundries - 1);

    /*write huffman header info*/
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::cout << "writing bin length value: " << huffman_code_vector[i].length << " length: " << SCALE_BITS_IN_HEADER << "\n";
      dataWriter.WriteBits(huffman_code_vector[i].length, SCALE_BITS_IN_HEADER);//5 bins for y, so the max length can only be 5, 3 bits are enough
    }

    /*write huffman code + data*/
    for(uint i = 0; i < kps.size(); i++){
      int symbol = 0;
      int value = 0;
      int code_length = 0;
      int num_length = 0;
      for(int j = 1; j < scale_info.number_of_boundries; j++){
        // std::cout << "number: " << kps[i].fsize  << " " << scale_info.boundary[j - 1] << " " << scale_info.boundary[j] << "\n";
        if( (kps[i].fsize >= scale_info.boundary[j - 1]) && (kps[i].fsize < scale_info.boundary[j])){
          symbol = j - 1;
        }else{
          continue;
        }
        // std::cout << "symbol: " << symbol << "\n";
        value = huffman_code_vector[symbol].value;
        code_length = huffman_code_vector[symbol].length;
        num_length = scale_info.bits_per_bin[symbol];
        // std::cout << "num_length: " << num_length << "\n";
        /*write huffman prefix*/
        // std::cout << "writing huffman code prefix: " << value << " length: " << code_length << "\n";
        dataWriter.WriteBits(value, code_length);
        /*write number itself*/
        // std::cout << "writing number: " << kps[i].fsize - scale_info.boundary[symbol] << " length: " << num_length << "\n";
        dataWriter.WriteBits(kps[i].fsize - scale_info.boundary[symbol], num_length);
      }
    }
    // /*flush the data*/
    // dataWriter.FlushBuffer();
    // // dataWriter.DisplayData();
    // const std::string path = "File718";
    // dataWriter.WriteToDisk(path);
    // dataWriter.ClearBuffer();
}

void Compressor::compressAngle(std::vector<KeypointEntry> kps){
    for(uint i = 0; i < kps.size(); i++){
      // std::cout << kps[i].codeword_vector[0] << " ";
      dataWriter.WriteBits(kps[i].angle, ANGLE_MAX_BITS);
    }
    // std::cout << "\n";
}

void Compressor::compressCW(std::vector<KeypointEntry> kps){
    for(uint i = 0; i < kps.size(); i++){
      // std::cout << kps[i].codeword_vector[0] << " ";
      dataWriter.WriteBits(kps[i].codeword_vector[0], CW_MAX_BITS);
    }
    // std::cout << "\n";
}

void Compressor::compressAttributes(){
	for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {

		/*prepare current document content*/
		std::vector<KeypointEntry> kps = it->second;

		/*sort by xysize*/
		std::sort(kps.begin(), kps.end(), SortByXYSCALE);
		// kps.resize(20);

		/*write out image length*/
		int image_length = kps.size();
		dataWriter.WriteBits(image_length, 11);
		std::cout << "image length: " << image_length << "\n";

		/*write out bit stream, 1 means unique, 0 means duplicate*/
		dataWriter.WriteBits(1, 1); // the first data must be unique
		for(uint i = 1; i < kps.size(); i++){
			if( equalsOnXYScale(kps[i], kps[i-1]) ){
				// std::cout << "equals\n";
				dataWriter.WriteBits(0, 1);
			}else{
				// std::cout << "not equals\n";
				dataWriter.WriteBits(1, 1);
			}
		}

		/*process anlge*/
    compressAngle(kps);

		/*process cw*/
    compressCW(kps);

		/*remove duplicates*/
		kps.erase( unique( kps.begin(), kps.end(), UniqueByXYScale), kps.end() );

    /*compress x*/
    compressX(kps);
    /*compress y*/
    compressY(kps);
    /*compress scale*/
    compressScale(kps);

    /*flush the data*/
    dataWriter.FlushBuffer();
    // dataWriter.DisplayData();
    const std::string path = "File724";
    dataWriter.WriteToDisk(path);
    dataWriter.ClearBuffer();
	}
 //    /*flush the data*/
	// dataWriter.FlushBuffer();
	// // dataWriter.DisplayData();
	// const std::string path = "File718";
	// dataWriter.WriteToDisk(path);
}

void Compressor::display(){
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
      /*prepare current document content*/
      std::vector<KeypointEntry> kps = it->second;

      /*sort by xysize*/
      std::sort(kps.begin(), kps.end(), SortByXYSCALE);

      for(uint i = 0; i < kps.size(); i++){
        std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
        << kps[i].codeword_vector[0] << "\n";
      }
      std::cout << "\nsize of the document: " << kps.size();
      std::cout << "\n-----end-of-document-----\n";
  }
}

void Compressor::LoadBinaryDataFromFile(){
  int file_count = 0;

  FILE *fp = fopen(this->binaryDocumentFile.c_str(), "rb");
  if (!fp){
    std::cout << this->binaryDocumentFile << " file not found\n";
  }
  int fversion = 0;
    size_t objects_read = fread(&fversion, 4, 1, fp);
    fversion = ntohl(fversion);
    if(fversion != 1){
      std::cerr << "data not version 1: " << fversion << std::endl;
      fclose(fp);
    }
    // std::cout << "data version: " << fversion << std::endl;

    const size_t max_id_length = 1000;
    char *idbuf = new char[max_id_length];
    while (!feof(fp)) {

      uint32_t id_size;
      size_t b_read = fread(&id_size, 4, 1, fp);
      if (b_read != 1) {  // this is actually where eof should occur
        break;
      }
      id_size = ntohl(id_size);
      if (id_size > max_id_length) {
        // LOG_WARN << "Stopping Reading rerank index, id length of " <<
        // id_size;
        break;
      }
      objects_read = fread(idbuf, id_size, 1, fp);
      if (objects_read != 1) {
        break;
      }

      std::string id(idbuf, id_size);
      size_t nbytes;
      objects_read = fread(&nbytes, 4, 1, fp);
      if (objects_read != 1) {
        break;
      }
      nbytes = ntohl(static_cast<uint32_t>(nbytes));
      if (nbytes > 50000) {
        // LOG_WARN << "rerank index read giving up, codeword keypoint length of
        // " << nbytes << " for Id: " << id;
        break;
      }
      std::unique_ptr<unsigned char[]> optr(new unsigned char[nbytes]);
      unsigned char *ptr = static_cast<unsigned char *>(optr.get());
      objects_read = fread(ptr, 1, nbytes, fp);
      if (objects_read < nbytes) {
        // vs_dict_feat does not have access to qrs LOG_WARN...
        // LOG_WARN << "rerank index read did not read full data for Id: "
        // << id;
        break;
      }
      while (ptr + 8 < static_cast<unsigned char *>(optr.get()) + nbytes) {
        // ptr +8 because absolute minimum number of bytes that will
        // be read is 9, this way we allow for data that has been padded
        int bytes_read = ParseMultipleCodewordKeypoints(id, ptr);
        ptr += bytes_read;
      }

      // std::cout << "\n";
      // std::cout << file_count << "\n";
      file_count++;
      // if( file_count > 500000){
      // if( file_count > 1){
      // if( file_count > 10){
      // 	break;
      // }

    }
    delete[] idbuf;

    fclose(fp);

    // std::cout << "total number of images in main memory for Forward Index: " << file_count << "\n";
}


int Compressor::ParseMultipleCodewordKeypoints(std::string id,
                                                unsigned char *optr) {

    unsigned char *ptr = optr;
    uint16_t x = ntohs(*reinterpret_cast<int16_t *>(ptr));
    ptr += 2;
    uint16_t y = ntohs(*reinterpret_cast<int16_t *>(ptr));
    ptr += 2;
    uint16_t angle = ntohs(*reinterpret_cast<int16_t *>(ptr));
    ptr += 2;
    uint16_t fsize = ntohs(*reinterpret_cast<int16_t *>(ptr));
    ptr += 2;
    unsigned char codewordcount = *ptr;
    ptr++;
    std::vector<int> codewords_tmp;
    while (codewordcount > 0) {
      unsigned int codeword = *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      // std::cout << x << "," << y << "," << fsize << "," << angle << "," << (int)codewordcount << "," <<codeword << " ";
      codewordcount--;
      codewords_tmp.push_back(codeword);
      // codewords_tmp.push_back(order_table[codeword]);
      // cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
    }
    keypoint_entry_byname_[id].emplace_back(codewords_tmp, x, y, fsize, angle);

    return static_cast<int>(ptr - optr);
}