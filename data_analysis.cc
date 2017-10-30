/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */
#include "data_analysis.h"
#include <iostream>
#include <fstream>
#include <netinet/in.h>
#include <bitset>
#include <cstring>
#include <memory>

void DataAnalyser::originalLoading(){
	FILE *fp = fopen(this->binaryDocumentFile.c_str(), "rb");
	if (!fp){
		std::cout << this->binaryDocumentFile << " file not found\n";
	}
	int fversion = 0;
  	size_t objects_read = fread(&fversion, 4, 1, fp);
  	fversion = ntohl(fversion);
  	if(fversion != 1){
  		std::cerr << "data not version 1: " << fversion << std::endl;
    	fclose(fp);
  	}
  	std::cout << "data version: " << fversion << std::endl;

    const size_t max_id_length = 1000;
    char *idbuf = new char[max_id_length];
    while (!feof(fp)) {
     //this varibale is used to record how many keypoints per image has, commend by me
     //since each keypoint may correspond to several codeword, commend by me
      this->keypoint_number_per_image = 0;
      uint32_t id_size;
      size_t b_read = fread(&id_size, 4, 1, fp);
      if (b_read != 1) {  // this is actually where eof should occur
        break;
      }
      id_size = ntohl(id_size);
      if (id_size > max_id_length) {
        // LOG_WARN << "Stopping Reading rerank index, id length of " <<
        // id_size;
        break;
      }
      objects_read = fread(idbuf, id_size, 1, fp);
      if (objects_read != 1) {
        break;
      }

      std::string id(idbuf, id_size);
      size_t nbytes;
      objects_read = fread(&nbytes, 4, 1, fp);
      if (objects_read != 1) {
        break;
      }
      nbytes = ntohl(static_cast<uint32_t>(nbytes));
      if (nbytes > 50000) {
        // LOG_WARN << "rerank index read giving up, codeword keypoint length of
        // " << nbytes << " for Id: " << id;
        break;
      }
      std::unique_ptr<unsigned char[]> optr(new unsigned char[nbytes]);
      unsigned char *ptr = static_cast<unsigned char *>(optr.get());
      objects_read = fread(ptr, 1, nbytes, fp);
      if (objects_read < nbytes) {
        // vs_dict_feat does not have access to qrs LOG_WARN...
        // LOG_WARN << "rerank index read did not read full data for Id: "
        // << id;
        break;
      }
      while (ptr + 8 < static_cast<unsigned char *>(optr.get()) + nbytes) {
        // ptr +8 because absolute minimum number of bytes that will
        // be read is 9, this way we allow for data that has been padded
        int bytes_read = ReadMultipleCodewordkeypoints(id, ptr);
        ptr += bytes_read;
      }
    }
    delete[] idbuf;

    fclose(fp);

    std::cout << "total number of bytes in main memory for Forward Index: " << cwkeypoint_byname_.size() * 12 << "\n";
}

int DataAnalyser::ReadMultipleCodewordkeypoints(std::string id,
                                                unsigned char *optr) {
	  std::vector<CodewordKeyPoint> cwkp_vec;
	  auto id_vec_it = cwkeypoint_byname_.find(id);
	  if (id_vec_it == cwkeypoint_byname_.end()) cwkeypoint_byname_[id] = cwkp_vec;

	  unsigned char *ptr = optr;
	  uint16_t x = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t y = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t angle = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t fsize = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  unsigned char codewordcount = *ptr;
	  ptr++;
	  while (codewordcount > 0) {
	    unsigned int codeword = *ptr;
	    ptr++;
	    codeword = (codeword << 8) | *ptr;
	    ptr++;
	    codeword = (codeword << 8) | *ptr;
	    ptr++;
	    codewordcount--;
	    cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
	  }

	  return static_cast<int>(ptr - optr);
}


int DataAnalyser::ReadMultipleCodewordkeypointsQi(std::string id,
                                                unsigned char *optr) {
	  std::vector<CodewordKeyPoint> cwkp_vec;
	  auto id_vec_it = cwkeypoint_byname_.find(id);
	  if (id_vec_it == cwkeypoint_byname_.end()) cwkeypoint_byname_[id] = cwkp_vec;

	  unsigned char *ptr = optr;
	  uint16_t x = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t y = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t angle = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  uint16_t fsize = ntohs(*reinterpret_cast<int16_t *>(ptr));
	  ptr += 2;
	  unsigned char codewordcount = *ptr;
	  this->keypoint_number_per_image += 1; //don't care about the value of codewordcount, treat it as 1 here, comment by me
	  x_vec.push_back(x);
	  y_vec.push_back(y);
	  angle_vec.push_back(angle);
	  size_vec.push_back(fsize);
	  ptr++;
	  while (codewordcount > 0) {
	    unsigned int codeword = *ptr;
	    ptr++;
	    codeword = (codeword << 8) | *ptr;
	    ptr++;
	    codeword = (codeword << 8) | *ptr;
	    ptr++;
	    codewordcount--;
	    cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
	    codeword_vec.push_back(codeword);
	  }

	  return static_cast<int>(ptr - optr);
}

void DataAnalyser::loadData(){
	FILE *fp = fopen(this->binaryDocumentFile.c_str(), "rb");
	if (!fp){
		std::cout << this->binaryDocumentFile << " file not found\n";
	}
	int fversion = 0;
  	size_t objects_read = fread(&fversion, 4, 1, fp);
  	fversion = ntohl(fversion);
  	if(fversion != 1){
  		std::cerr << "data not version 1: " << fversion << std::endl;
    	fclose(fp);
  	}
  	std::cout << "data version: " << fversion << std::endl;

    const size_t max_id_length = 1000;
    char *idbuf = new char[max_id_length];
    while (!feof(fp)) {
     //this varibale is used to record how many keypoints per image has, commend by me
     //since each keypoint may correspond to several codeword, commend by me
      this->keypoint_number_per_image = 0;
      uint32_t id_size;
      size_t b_read = fread(&id_size, 4, 1, fp);
      if (b_read != 1) {  // this is actually where eof should occur
        break;
      }
      id_size = ntohl(id_size);
      if (id_size > max_id_length) {
        // LOG_WARN << "Stopping Reading rerank index, id length of " <<
        // id_size;
        break;
      }
      objects_read = fread(idbuf, id_size, 1, fp);
      if (objects_read != 1) {
        break;
      }

      std::string id(idbuf, id_size);
      size_t nbytes;
      objects_read = fread(&nbytes, 4, 1, fp);
      if (objects_read != 1) {
        break;
      }
      nbytes = ntohl(static_cast<uint32_t>(nbytes));
      if (nbytes > 50000) {
        // LOG_WARN << "rerank index read giving up, codeword keypoint length of
        // " << nbytes << " for Id: " << id;
        break;
      }
      std::unique_ptr<unsigned char[]> optr(new unsigned char[nbytes]);
      unsigned char *ptr = static_cast<unsigned char *>(optr.get());
      objects_read = fread(ptr, 1, nbytes, fp);
      if (objects_read < nbytes) {
        // vs_dict_feat does not have access to qrs LOG_WARN...
        // LOG_WARN << "rerank index read did not read full data for Id: "
        // << id;
        break;
      }
      while (ptr + 8 < static_cast<unsigned char *>(optr.get()) + nbytes) {
        // ptr +8 because absolute minimum number of bytes that will
        // be read is 9, this way we allow for data that has been padded
        int bytes_read = ReadMultipleCodewordkeypointsQi(id, ptr);
        ptr += bytes_read;
      }
      auto id_it = keypoint_number_byname_.find(id);
      if (id_it == keypoint_number_byname_.end()) keypoint_number_byname_[id] = this->keypoint_number_per_image;
      else {
      	std::cout<<"duplicate imageID detected, something is wrong\n";
      	exit(0);
      }
    }
    delete[] idbuf;

    fclose(fp);

    std::cout << cwkeypoint_byname_.size() << "\n";
}

void DataAnalyser::getkeypointDistribution(){
	FILE * fileDir = fopen(this->keypointOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->keypointOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "imageID,keypointLength\n");
	for ( auto it = keypoint_number_byname_.begin(); it != keypoint_number_byname_.end(); ++it ){
		fprintf(fileDir, "%s,%u\n", it->first.c_str(), it->second);
	}
	fclose(fileDir);
}

void DataAnalyser::getcodewordDistribution(){
	FILE * fileDir = fopen(this->codewordOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->codewordOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "imageID,codewordLength\n");
	for ( auto it = cwkeypoint_byname_.begin(); it != cwkeypoint_byname_.end(); ++it ){
		fprintf(fileDir, "%s,%lu\n", it->first.c_str(), it->second.size());
	}
	fclose(fileDir);
}

void DataAnalyser::getXCoordinateDistribution(){
	FILE * fileDir = fopen(this->XCoordinateOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->XCoordinateOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "x\n");
	for ( unsigned int i = 0; i < x_vec.size(); i++ ){
		fprintf(fileDir, "%u\n", x_vec[i]);
	}
	fclose(fileDir);
}

void DataAnalyser::getYCoordinateDistribution(){
	FILE * fileDir = fopen(this->YCoordinateOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->YCoordinateOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "y\n");
	for ( unsigned int i = 0; i < y_vec.size(); i++ ){
		fprintf(fileDir, "%u\n", y_vec[i]);
	}
	fclose(fileDir);
}

void DataAnalyser::getAngleDistribution(){
	FILE * fileDir = fopen(this->angleOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->angleOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "angle\n");
	for ( unsigned int i = 0; i < angle_vec.size(); i++ ){
		fprintf(fileDir, "%u\n", angle_vec[i]);
	}
	fclose(fileDir);
}

void DataAnalyser::getSizeDistribution(){
	FILE * fileDir = fopen(this->sizeOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->sizeOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "size\n");
	for ( unsigned int i = 0; i < size_vec.size(); i++ ){
		fprintf(fileDir, "%u\n", size_vec[i]);
	}
	fclose(fileDir);
}

void DataAnalyser::getcodewordValueDistribution(){
	FILE * fileDir = fopen(this->codewordValueOutputFile.c_str(), "w");
	if( fileDir == NULL){
	   std::cout << "Problem! The file: " << this->codewordValueOutputFile << " could not be opened!" << std::endl;
	   exit(0);
	}
	fprintf(fileDir, "codewordvalue\n");
	for ( unsigned int i = 0; i < codeword_vec.size(); i++ ){
		fprintf(fileDir, "%d\n", codeword_vec[i]);
	}
	fclose(fileDir);
}

void DataAnalyser::readSingleCodewordKeypoint(){
	//the input, 12 bytes data
	char * ptr = new char[12];
	ptr[0] = 0b00000010;
	ptr[1] = 0b00000001;
	ptr[2] = 0b00000001;
	ptr[3] = 0b00000001;
	ptr[4] = 0b00000001;
	ptr[5] = 0b00000001;
	ptr[6] = 0b00000001;
	ptr[7] = 0b00000001;
	ptr[8] = 0b00000001;
	ptr[9] = 0b00000001;
	ptr[10] = 0b00000001;
	ptr[11] = 0b00000001;

	int original = ntohl(*reinterpret_cast<const unsigned int *>(ptr));
	std::string binary = std::bitset<32>(original).to_string(); //to binary
    std::cout << "original 4 bytes data: \n" << original << "\nin binary format: \n" << binary << "\n";

	int codeword = ntohl(*reinterpret_cast<const unsigned int *>(ptr)) & 0x0FFFFFF;
	binary = std::bitset<32>(codeword).to_string(); //to binary
    std::cout << "codeword: \n" << codeword << "\nin binary format: \n" << binary << "\n";

	int size = static_cast<unsigned char>(ntohl(*reinterpret_cast<const unsigned int *>(ptr)) >> 24);
	binary = std::bitset<32>(size).to_string(); //to binary
	std::cout << "size: \n" << size << "\nin binary format: \n" << binary << "\n";

	uint16_t fsize = static_cast<uint16_t>(size * 128);  // fixed point style
	binary = std::bitset<16>(fsize).to_string(); //to binary
	std::cout << "fsize: \n" << fsize << "\nin binary format: \n" << binary << "\n";

	const char* cptr = reinterpret_cast<const char *>(ptr);
	cptr = reinterpret_cast<const char *>(cptr) + 4;
	int16_t x = ntohs(*reinterpret_cast<const int16_t *>(cptr));
	binary = std::bitset<16>(x).to_string(); //to binary
	std::cout << "x: \n" << x << "\nin binary format: \n" << binary << "\n";

	cptr = reinterpret_cast<const char *>(cptr) + 2;
	int16_t y = ntohs(*reinterpret_cast<const int16_t *>(cptr));
	binary = std::bitset<16>(y).to_string(); //to binary
	std::cout << "y: \n" << y << "\nin binary format: \n" << binary << "\n";

	cptr = reinterpret_cast<const char *>(cptr) + 2;
	uint16_t angle = ntohs(*reinterpret_cast<const int16_t *>(cptr));
	binary = std::bitset<16>(angle).to_string(); //to binary
	std::cout << "angle: \n" << angle << "\nin binary format: \n" << binary << "\n";
}