#include <string>
#include <vector>
#include "entry.h"
#include "../include/rerank_keypoints.h"

class reorder{
public:
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bowir_20170607_rerank.bin";
	const std::string codeword_mapping = "/mnt3/qi/indexData/codeword_mapping_file";

	// std::unordered_map< std::string, std::vector<CodewordEntry> > codeword_entry_byname_;
	// std::unordered_map< std::string, std::vector<CodewordKeyPoint> > cwkeypoint_byname_;
	std::vector<CodewordIndexEntry> codeword_table;

	void Order();

	void LoadBinaryDataFromFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	void InitCodewordTable();
	void ReorderTable();
	void DisplayCodewordTable();
	void WriteTable();

	const std::string table_file = "/mnt3/qi/indexData/codeword_mapping_bespoke";
	std::vector<int> order_table;
	void RebuildIndex();
	void LoadMapper();
};