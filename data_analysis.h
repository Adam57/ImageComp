/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */
#include <string>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include "../include/rerank_keypoints.h"

class DataAnalyser{
public:
	const std::string binaryDocumentFile = "/mnt3/qi/indexData/bespoke_20170606_rerank.bin";
	const std::string codewordOutputFile = "/mnt3/qi/distributionData/codewordLength.csv";
	const std::string keypointOutputFile = "/mnt3/qi/distributionData/keypointLength.csv";
	const std::string XCoordinateOutputFile = "/mnt3/qi/distributionData/XCoordinates.csv";
	const std::string YCoordinateOutputFile = "/mnt3/qi/distributionData/YCoordinates.csv";
	const std::string angleOutputFile = "/mnt3/qi/distributionData/angles.csv";
	const std::string sizeOutputFile = "/mnt3/qi/distributionData/sizes.csv";
	const std::string codewordValueOutputFile = "/mnt3/qi/distributionData/codewordValue.csv";

	std::unordered_map< std::string, std::vector<CodewordKeyPoint> > cwkeypoint_byname_;
	std::unordered_map< std::string, int> keypoint_number_byname_;
	// std::unordered_map< std::string, int> x_byname_;
	// std::unordered_map< std::string, int> y_byname_;
	std::vector<uint16_t> x_vec;
	std::vector<uint16_t> y_vec;
	std::vector<uint16_t> angle_vec; 
	std::vector<uint16_t> size_vec;
	std::vector<uint32_t> codeword_vec;
	int keypoint_number_per_image;

	void readSingleCodewordKeypoint();
	void loadData();
	int ReadMultipleCodewordkeypoints(std::string id,
                                      unsigned char *optr);
	int ReadMultipleCodewordkeypointsQi(std::string id,
                                      unsigned char *optr);
	void getcodewordDistribution();
	void getkeypointDistribution();
	void getXCoordinateDistribution();
	void getYCoordinateDistribution();
	void getAngleDistribution();
	void getSizeDistribution();
	void getcodewordValueDistribution();

	void originalLoading();
	void newLoading();

};