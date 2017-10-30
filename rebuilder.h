#include <string>
#include <vector>
#include "entry.h"
#include "../include/rerank_keypoints.h"

class rebuilder{
public:
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bowir_20170607_rerank.bin";
	void LoadBinaryDataFromFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	/*loading the data*/
	const std::string table_file = "/mnt3/qi/indexData/codeword_mapping_bespoke";
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

};