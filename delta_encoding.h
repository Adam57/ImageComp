#include <string>
#include <vector>
#include "../include/rerank_keypoints.h"
#include "entry.h"

class DeltaEncoding{
public: 
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	const std::string deltaDocumentFile = "/mnt3/qi/indexData/delta_bespoke_20170606_rerank.bin";

	std::unordered_map< std::string, std::vector<CodewordKeyPoint> > cwkeypoint_byname_;
	void LoadBinaryDataFromFile();
	void SaveBinaryDataToFile();
	int ParseMultipleCodewordKeypoints(std::string id, unsigned char *optr);

	std::unordered_map< std::string, std::vector<KeypointEntry> > keypoint_entry_byname_;
	void LoadBinaryDataFromFileForSorting();
	void SaveBinaryDataToFileForSorting();
	int ParseMultipleCodewordKeypointsForSorting(std::string id, unsigned char *optr);

	void Encoding();

	void SortByCoordinateXAndDelta();
	void GetDeltaXCoordinateDistribution();
	const std::string XCoordinateOutputFile = "/mnt3/qi/distributionData/DeltaXCoordinates.csv";
	std::vector<int> delta_x_vector;

	void SortByCoordinateXThenYAndDelta();
	void GetDeltaYCoordinateDistribution();
	const std::string YCoordinateOutputFile = "/mnt3/qi/distributionData/DeltaYByXOrderCoordinates.csv";
	std::vector<int> delta_y_vector;

	void SortByCodewordAndDelta();
	void GetDeltaCodewordDistribution();
	const std::string CodewordOutputFile = "/mnt3/qi/distributionData/DeltaCodeWord.csv";
	std::vector<int> delta_codeword_vector;
	std::unordered_map< std::string, std::vector<CodewordEntry> > codeword_entry_byname_;	

	void SortByFirstCodewordRawValue();
	void GetFirstRawCodewordDistribution();
	std::unordered_map< std::string, std::vector<int> > codeword_only_byname_;
	std::vector<int> codeword_raw_vector;	
	const std::string CodewordRawValueOutputFile = "/mnt3/qi/distributionData/CodeWordRawValue.csv";

	void SortByCodewordLength();
	void GetCodewordLengthDistribution();
	void GetCodewordLengthDeltaDistribution();
	const std::string CodewordLengthOutputFile = "/mnt3/qi/distributionData/UniqueCodeWordLength.csv";
	const std::string DeltaCodewordLengthOutputFile = "/mnt3/qi/distributionData/DeltaCodeWordLength.csv";
	std::vector<int> unique_codeword_length_vector;

	void GetDocument();
	const std::string DocumentPath = "/mnt3/qi/distributionData/image";

	void GetDeltaList(std::vector<int>& list);

	// void SortByFirstCodewordAndDelta();
	// void GetDeltaFirstCodewordDistribution();
	// const std::string CodewordOutputFile = "/mnt3/qi/distributionData/DeltaCodeWord.csv";
	// std::vector<int> delta_codeword_vector;
	// std::unordered_map< std::string, std::vector<CodewordEntry> > codeword_entry_byname_;	

};