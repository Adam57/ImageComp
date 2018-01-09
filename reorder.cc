#include "reorder.h"
#include <netinet/in.h>
#include <memory>
#include <iostream>
#include <assert.h>
#include <algorithm>

bool SortByFreq(CodewordIndexEntry i, CodewordIndexEntry j){
  return (i.count > j.count);
}

bool SortByFreqAngle(AngleIndexEntry i, AngleIndexEntry j){
  return (i.count > j.count);
}

void reorder::remap_cw(){
	InitCodewordTable();
	LoadBinaryDataFromFile();
	ReorderTable();
	// DisplayCodewordTable();
	WriteTable();
}

void reorder::remap_angle(){
  InitAngleTable();
  LoadBinaryDataFromFile();
  ReorderAngleTable();
  WriteAngleTable();
}

void reorder::InitCodewordTable(){
	/*The codeword currently has 2M distinct values*/
	int codeword_table_size = 2000000;
	codeword_table.reserve(codeword_table_size);
	for(int i = 0; i < codeword_table_size; i++){
		CodewordIndexEntry e(i, 0);
		codeword_table.push_back(e);
	}
}

void reorder::InitAngleTable(){
  /*The Angle currently has 2M distinct values*/
  int angle_table_size = 65536;
  angle_table.reserve(angle_table_size);
  for(int i = 0; i < angle_table_size; i++){
    AngleIndexEntry e(i, 0);
    angle_table.push_back(e);
  }
}

void reorder::ReorderTable(){
	std::sort(codeword_table.begin(), codeword_table.end(), SortByFreq);
}

void reorder::ReorderAngleTable(){
  std::sort(angle_table.begin(), angle_table.end(), SortByFreqAngle);
}

void reorder::DisplayCodewordTable(){
	for(int i = 0; i < 200; i++){
		std::cout <<codeword_table[i].codeword << " " << codeword_table[i].count << "\n";
	}
}

void reorder::WriteTable(){
	FILE * pFile;
	pFile = fopen (this->codeword_mapping.c_str(), "w");
	for(int i = 0; i < codeword_table.size(); i++){
		fprintf(pFile, "%d %lu\n", codeword_table[i].codeword, codeword_table[i].count);
	}
	fclose(pFile);
}

void reorder::WriteAngleTable(){
  FILE * pFile;
  pFile = fopen (this->angle_mapping.c_str(), "w");
  for(int i = 0; i < angle_table.size(); i++){
    fprintf(pFile, "%d %lu\n", angle_table[i].angle, angle_table[i].count);
  }
  fclose(pFile);
}

void reorder::LoadBinaryDataFromFile(){
  int file_count = 0;

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
        int bytes_read = ParseMultipleCodewordKeypoints(id, ptr);
        ptr += bytes_read;
      }

      // std::cout << "\n";
      file_count++;
      // if( file_count > 9){
      // 	break;
      // }

    }
    delete[] idbuf;

    fclose(fp);

    std::cout << "total number of images in main memory for Forward Index: " << file_count << "\n";
}


int reorder::ParseMultipleCodewordKeypoints(std::string id,
                                                unsigned char *optr) {
    // std::vector<CodewordKeyPoint> cwkp_vec;
    // auto id_vec_it = cwkeypoint_byname_.find(id);
    // if (id_vec_it == cwkeypoint_byname_.end()) cwkeypoint_byname_[id] = cwkp_vec;

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
      // std::cout << x << "," << y << "," << fsize << "," << angle << "," << (int)codewordcount << "," <<codeword << " ";
      codewordcount--;
      // codeword_table[codeword].count++;
      angle_table[angle].count++;
      // cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
    }

    return static_cast<int>(ptr - optr);
}