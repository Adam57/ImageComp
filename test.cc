/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */
// #include "data_analysis.h"
#include "entropy.h"
#include "variable_byte_encoding.h"
// #include "delta_encoding.h"
// #include "reorder.h"
#include "rebuilder.h"
#include "compressor.h"
#include "var_bits.h"
#include "huffman.h"
#include <iostream>
#include <bitset>

int main(int argc, char* argv[]){

	std::string command(argv[1]);
	/*Data Analysis*/
	// DataAnalyser c;
	// c.readSingleCodewordKeypoint();

	/*run loadData first before run the following test*/
	// c.loadData();

	/*run loadData above before run the following test*/
	// c.getcodewordDistribution();
	// c.getkeypointDistribution();
	// c.getXCoordinateDistribution();
	// c.getYCoordinateDistribution();
	// c.getAngleDistribution();
	// c.getSizeDistribution();
	// c.getcodewordValueDistribution();

	/*loading test*/
	// c.originalLoading();

	/*entropy test*/
	// Entropy e;
	// const int length = 5; //length is the buffer size, total occurence of all the chars

	// uint32_t buf[5];
	// buf[0] = 1;
	// buf[1] = 2;
	// buf[2] = 2;
	// buf[3] = 2;
	// buf[4] = 2;
	// e.shannon_test(buf, length);

	/*get dataset entropy*/
	// Entropy e;
	// e.shannon_entropy_from_file();

	/*varbyte test*/
	// VarByteEncoding v;
	// v.encoding();

	/*delta test*/
	// DeltaEncoding d;
	// d.Encoding();

	/*get new order*/
	//reorder r;
	//r.Order();

	/*rebuild index*/
	// rebuilder r;
	// r.RebuildIndex();

	/*bits writer and reader*/
	if(command == "bits"){
		BitsWriter bw;
		bw.test();

		uint size = bw.data.size();
		unsigned char* data = new unsigned char[size];
		for(uint i = 0; i < size; i++){
			data[i] = bw.data[i];
		}

		BitsReader br(data, size);
		br.test();
	}

	/*var bits*/
	// VarBits vb(4, 5);
	// std::vector<uint> numbers;
 //  	numbers.push_back(15);
 //  	numbers.push_back(14);
 //  	numbers.push_back(6);
 //  	numbers.push_back(307);
 //  	numbers.push_back(301);
 //  	numbers.push_back(0);

 //  	std::vector<unsigned char> compressed_data = vb.compress(numbers);

 //  	uint size = compressed_data.size();
	// unsigned char* data = new unsigned char[size];
	// for(uint i = 0; i < size; i++){
	// 	data[i] = compressed_data[i];
	// 	std::string binary = std::bitset<8>(data[i]).to_string(); //to binary
 //    	std::cout << binary << "\n";
	// }


	// std::vector<int> results = vb.decompress(data, size);
	// for(uint i = 0; i < results.size(); i++){
	// 	std::cout << results[i] << "\n";
	// }

	/*verify reps*/
	// rebuilder r;
	// r.VerifyReps();

	/*huffman*/
	// int output[18];
	// int input[18];
	// input[0] = 7;
	// input[1] = 0;
	// input[2] = 0;
	// input[3] = 0;
	// input[4] = 0;
	// input[5] = 2;
	// input[6] = 3;
	// input[7] = 13;
	// input[8] = 14;
	// input[9] = 27;
	// input[10] = 49;
	// input[11] = 96;
	// input[12] = 103;
	// input[13] = 104;
	// input[14] = 55;
	// input[15] = 6;
	// input[16] = 0;
	// input[17] = 1;
	// std::cout << "start\n";
	// Huffman_Canonical(input, 18, output);

	// for(int i = 0; i < 18; i++){
	// 	std::cout << i+1 << ": "<< output[i] << "\n";
	// }

	/*Huffman for Codeword*/
	// rebuilder r;
	// r.HuffmanForCodeword();

	/*Huffman for scale*/
	// rebuilder r;
	// r.HuffmanForScale();

	/*Huffman for x*/
	// rebuilder r;
	// r.HuffmanForX();

	/*Huffman for y*/
	// rebuilder r;
	// r.HuffmanForY();

	/*Huffman for angle*/
	if( command == "angle"){
		rebuilder r;
		r.HuffmanForAngle();
	}

	/*compress the size*/
	// rebuilder r;
	// r.CompressSize();

	/*compression*/
	if( command == "c"){
		Compressor c;
		c.compress();
	}

	/*decompression*/
	if( command == "d"){
		Decompressor dc;
		dc.decompress();
	}

	/*display documents*/
	if( command == "display"){
		Compressor c;
		c.display();
	}
	return 0;
}