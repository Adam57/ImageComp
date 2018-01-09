/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */
#include <string>
#include <stdint.h>
#include <unordered_map>
#include <vector>

class CodewordKeyPoint {
 public:
 	  CodewordKeyPoint(int cw1, int16_t x1, int16_t y1, uint16_t sf1,
                   uint16_t a1);
  int codeword;  // 3 bytes allowed
  int16_t x;
  int16_t y;
  uint16_t fsize;  // fixed point representation (size * 128)
  uint16_t angle;  // fixed point representation
};

class DataAnalyser{
public:
	const std::string binaryDocumentFile = "/home/qw376/fic_data/rerank.bin";
	const std::string codewordOutputFile = "/home/qw376/fic_data/csv_files/codewordLength.csv";
	const std::string codewordValueOutputFile = "/home/qw376/fic_data/csv_files/codewordValue.csv";
	const std::string cw_remap_file = "/home/qw376/fic_data/csv_files/cw_remap_file.csv";
	const std::string angle_remap_file = "/home/qw376/fic_data/csv_files/angle_remap_file.csv";
	const std::string XCoordinateOutputFile = "/home/qw376/fic_data/csv_files/XCoordinates.csv";
	const std::string YCoordinateOutputFile = "/home/qw376/fic_data/csv_files/YCoordinates.csv";
	const std::string angleOutputFile = "/home/qw376/fic_data/csv_files/angles.csv";
	const std::string sizeOutputFile = "/home/qw376/fic_data/csv_files/sizes.csv";
	const std::string table_file = "/home/qw376/fic_data/remappings/cw_mapping_file";
	const std::string angle_table_file = "/home/qw376/fic_data/remappings/angle_mapping_file";

	const std::string cw_remap_delta_file = "/home/qw376/fic_data/csv_files/cw_remap_delta_file.csv";

	std::unordered_map< std::string, std::vector<CodewordKeyPoint> > cwkeypoint_byname_;
	// std::unordered_map< std::string, int> keypoint_number_byname_;
	// std::unordered_map< std::string, int> x_byname_;
	// std::unordered_map< std::string, int> y_byname_;
	std::vector<uint16_t> x_vec;
	std::vector<uint16_t> y_vec;
	std::vector<uint16_t> angle_vec; 
	std::vector<uint16_t> size_vec;
	std::vector<uint32_t> codeword_vec;
	std::vector<int> codeword_vec_image;

	std::vector<int> order_table;
	std::vector<int> angle_table;
	// int keypoint_number_per_image;
	void LoadMapper();
	void LoadMapperAngle();
	void readSingleCodewordKeypoint();
	void loadData();
	int ReadMultipleCodewordkeypoints(std::string id,
                                      unsigned char *optr);
	int ReadMultipleCodewordkeypointsQi(std::string id,
                                      unsigned char *optr);
	int ReadMultipleCodewordkeypointsDelta(std::string id,
                                      unsigned char *optr);
	// void getcodewordDistribution();
	void getkeypointDistribution();
	void getXCoordinateDistribution();
	void getYCoordinateDistribution();
	void getAngleDistribution();
	void getSizeDistribution();
	void getcodewordValueDistribution();
	void cw_disremap();
	void angle_disremap();

	/*delta thing*/
	void loadData_delta();
	void cw_remap_delta();

	void originalLoading();
	void newLoading();

};