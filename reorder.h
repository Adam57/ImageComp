#include <string>
#include <vector>
#include "entry.h"
// #include "../include/rerank_keypoints.h"

class reorder{
public:
	const std::string binaryDocumentFile = "/home/qw376/fic_data/rerank.bin";
	// const std::string binaryDocumentFile = "/mnt3/qi/indexData/bowir_20170607_rerank.bin";
	// const std::string codeword_mapping = "/mnt3/qi/indexData/codeword_mapping_file";
	const std::string codeword_mapping = "/home/qw376/fic_data/remappings/cw_mapping_file";
	const std::string angle_mapping = "/home/qw376/fic_data/remappings/angle_mapping_file";

	// std::unordered_map< std::string, std::vector<CodewordEntry> > codeword_entry_byname_;
	// std::unordered_map< std::string, std::vector<CodewordKeyPoint> > cwkeypoint_byname_;
	std::vector<CodewordIndexEntry> codeword_table;
	std::vector<AngleIndexEntry> angle_table;

	void remap_cw();
	void remap_angle();

	void LoadBinaryDataFromFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	void InitCodewordTable();
	void ReorderTable();
	void DisplayCodewordTable();
	void WriteTable();


	void InitAngleTable();
	void ReorderAngleTable();
	void WriteAngleTable();

	const std::string table_file = "/mnt3/qi/indexData/codeword_mapping_bespoke";
	std::vector<int> order_table;
	void RebuildIndex();
	void LoadMapper();
};