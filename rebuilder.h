#include <string>
#include <vector>
#include <unordered_map>
#include "entry.h"
#include <fstream>
#include <bitset>
#include "mem.h"
#include "fse.h"
// #include "../include/rerank_keypoints.h"

class rebuilder{
public:
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bowir_20170607_rerank.bin";
	const std::string binaryDocumentFile = "/home/qw376/fic_data/rerank.bin";
	void LoadBinaryDataFromFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	/*loading the data*/
	const std::string table_file = "/home/qw376/fic_data/remappings/cw_mapping_file";
	std::vector<int> order_table;
	void RebuildIndex();
	void LoadMapper();


	/*sorting function*/
	std::unordered_map< std::string, std::vector<KeypointEntry> > keypoint_entry_byname_;
	void SortByFirstCodewordRawValue();
	void SortCodewordsWithinKeypoints();
	void SortByXThenYThenSize();
	void DisplayData();


	/*varbits and pfd for Scale*/
	std::unordered_map< std::string, SizeInfoEntry > sizeinfo_entry_byname_;
	void CompressSize();
	int GetPower(int number);
	int indexofSmallestElement(int array[], int size);
	void SubstractBaseValueForSize();
	void CaculateActualSize();

	/*replication deletion for x,y,size*/
	void VerifyReps();
	void RemoveRepsOriginalOrder();

	/*huffman for Codeword*/
	std::unordered_map< std::string, CodewordHuffmanInfoEntry > codeword_huffman_info_entry_byname_;
	void HuffmanForCodeword();
	void FillBinsHuffmanCodeword();
	void CaculateHuffmanCWSize();
	void GetCanonicalHuffmanCode(std::vector<CanonicalHuffmanNode>& huffman_code_vector, int huffman_bits_per_bin[], int size);

	/*huffman for Scale*/
	std::unordered_map< std::string, ScaleHuffmanInfoEntry > scale_huffman_info_entry_byname_;
	void HuffmanForScale();
	void FillBinsHuffmanScale();
	void CaculateHuffmanScaleSize();

	/*huffman for X*/
	std::unordered_map< std::string, XHuffmanInfoEntry > x_huffman_info_entry_byname_;
	void HuffmanForX();
	void FillBinsHuffmanX();
	void CaculateHuffmanXSize();

	/*huffman for X*/
	std::unordered_map< std::string, YHuffmanInfoEntry > y_huffman_info_entry_byname_;
	void HuffmanForY();
	void FillBinsHuffmanY();
	void CaculateHuffmanYSize();

	/*huffman for Angle*/
	std::unordered_map< std::string, AngleHuffmanInfoEntry > angle_huffman_info_entry_byname_;
	void HuffmanForAngle();
	void FillBinsHuffmanAngle();
	void CaculateHuffmanAngleSize();
	void LoadMapperAngle();
	std::vector<int> angle_table;
	const std::string angle_remap_file= "/home/qw376/fic_data/remappings/angle_mapping_file";

	/*vbyte*/
	void Vbyte();
	int compressionVbytes(std::vector<unsigned int> input);
	std::vector<unsigned uint> compressedList;
	int num_vbyte_vars;

	/*pford*/
	int pfdb[21];
	int threshold = 28;
	void pfd();
	int pfd_bits;
	int x_bits = 9;
	int y_bits = 9;
	int size_bits = 14;
	int angle_bits = 16;
	int cw_bits = 21;

	/*golomb*/
	void golomb();
	int golomb_bits;
	float a = 0.69;

	/*simple9*/
	void simple9();
	int simple9_bits;

	/*gzip*/
	void gzip();
	
	/*ans*/
	void ans();
};