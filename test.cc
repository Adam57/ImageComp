/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */
#include "data_analysis.h"
#include "entropy.h"
#include "variable_byte_encoding.h"
// #include "delta_encoding.h"
#include "reorder.h"
#include "rebuilder.h"
#include "compressor.h"
#include "var_bits.h"
#include "huffman.h"
#include <iostream>
#include <bitset>

int main(int argc, char* argv[]){

	std::string command(argv[1]);
	/*Data Analysis*/
	if(command == "test") {
		DataAnalyser d;
		d.readSingleCodewordKeypoint();
	}

	if(command == "imglength") {
		DataAnalyser d;
		d.loadData();
		d.getkeypointDistribution();
	}

	if(command == "codewordvalue") {
		DataAnalyser d;
		d.loadData();
		d.getcodewordValueDistribution();
	}

	if(command == "angle") {
		DataAnalyser d;
		d.loadData();
		d.getAngleDistribution();
	}

	if(command == "size") {
		DataAnalyser d;
		d.loadData();
		d.getSizeDistribution();
	}

	if(command == "x") {
        DataAnalyser d;
        d.loadData();
        d.getXCoordinateDistribution();
    }

   	if(command == "y") {
        DataAnalyser d;
        d.loadData();
        d.getYCoordinateDistribution();
    }

   	if(command == "remap_cw") {
		reorder r;
		r.remap_cw();
	}

	if(command == "remap_angle") {
		reorder r;
		r.remap_angle();
	}

	if(command == "remap_cw_dis") {
		DataAnalyser d;
		d.LoadMapper();
		d.loadData();
		d.cw_disremap();
	}

	if(command == "remap_angle_dis") {
		DataAnalyser d;
		d.LoadMapperAngle();
		d.loadData();
		d.angle_disremap();
	}


	/*get dataset entropy*/
	if(command == "cw_ave_en") {
		Entropy e;
		e.cw_avrage_entropy();
		e.get_cw_entropy_dis();
	}

	/*delta for code word*/
	if(command == "delta_cw") {
		DataAnalyser d;
		d.LoadMapper();
		d.loadData_delta();
		d.cw_remap_delta();
	}

	/*Huffman for angle*/
	if( command == "huff_angle"){
		rebuilder r;
		r.HuffmanForAngle();
	}

	/*Huffman for cw*/
	if( command == "huff_cw"){
		rebuilder r;
		r.HuffmanForCodeword();
	}

	/*Huffman for x*/
	if(command == "huff_x"){
		rebuilder r;
		r.HuffmanForX();
	}


	/*new varbyte for x y size*/
	if(command == "vbyte") {
		rebuilder r;
		r.Vbyte();
	}

	/*old varbyte for angle and cw*/
	if(command == "old_vbyte"){
		VarByteEncoding v;
		v.encoding();
	}

	/*pfd*/
	if(command == "pfd") {
		rebuilder r;
		r.pfd();
	} 

	/*Golomb*/
	if(command == "golomb") {
		rebuilder r;
		r.golomb();
	}

	/*Simple9*/
	if(command == "s9") {
		rebuilder r;
		r.simple9();
	}

	/*gzip*/
	if(command == "gzip") {
		rebuilder r;
		r.gzip();
	}

	/*ans*/
	if(command == "ans") {
		rebuilder r;
		r.ans();
	}

	// /*get entropy as a whole*/
	// if(command == "entropy") {
	// 	Entropy e;
	// 	e.shannon_entropy_from_file();
	// }


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

	/*delta test*/
	// DeltaEncoding d;
	// d.Encoding();

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