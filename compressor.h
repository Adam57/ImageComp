/*************************************************************************
* Name:        compressor.h
* Author:      Qi Wang
* Description: coder/decoder interface.
* Copyright:   2017 Blippar
*-------------------------------------------------------------------------
*************************************************************************/

#ifndef _compressor_h_
#define _compressor_h_

#include <string>
#include <vector>
#include "entry.h"
#include "bits_utils.h"
#include "../include/rerank_keypoints.h"

void GetCanonicalHuffmanCode(std::vector<CanonicalHuffmanNode>& huffman_code_vector, int huffman_bits_per_bin[], int size);

class Compressor{
public:
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bowir_20170607_rerank.bin";
	void LoadBinaryDataFromFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	std::unordered_map< std::string, std::vector<KeypointEntry> > keypoint_entry_byname_;
	BitsWriter dataWriter;

	/*x, y, size*/
	void compress();
	void compressAttributes();
	void compressX(std::vector<KeypointEntry> kps);
	void compressY(std::vector<KeypointEntry> kps);
	void compressScale(std::vector<KeypointEntry> kps);
	void compressCW(std::vector<KeypointEntry> kps);
	void compressAngle(std::vector<KeypointEntry> kps);
	bool equalsOnXYScale(KeypointEntry k1, KeypointEntry k2);
	void display();

};

class Decompressor{
public: 
	const std::string compressedFile = "File724";
	// BitsReader* dataReader;
	// unsigned char* input;
	// Decompressor();
	// ~Decompressor();
	/*x, y, size*/
	void decompress();
	void decompressX(std::vector<int>& x_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader);
	void decompressY(std::vector<int>& y_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader);
	void decompressScale(std::vector<int>& scale_vector, std::vector<int> bit_vector, int image_unique_length, BitsReader& dataReader);
	void decompressAngle(std::vector<int>& angle_vector, int image_length, BitsReader& dataReader);
	void decompressCW(std::vector<int>& angle_vector, int image_length, BitsReader& dataReader);
};

#endif