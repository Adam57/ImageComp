#include <string>
#include <vector>

class VarByteEncoding{
public:
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	const std::string varByteDocumentFile = "/mnt3/qi/indexData/varbyte_bespoke_20170606_rerank.bin";
	std::vector<unsigned char> compressedList;
	std::vector<unsigned uint> uncompressedList;
	int compressionVbytes(std::vector<unsigned int> input);
	int decompressionVbytesInt(unsigned char* input, unsigned * output, int size);
	void loadDataFromFile();
	int parse_multiple_codeword_keypoints(std::string id, unsigned char *optr);
	void encoding();
};