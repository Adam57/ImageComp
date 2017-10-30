#include "delta_encoding.h"
#include <netinet/in.h>
#include <memory>
#include <iostream>
#include <assert.h>
#include <algorithm>

bool SortByXForVector(KeypointEntry i, KeypointEntry j){
  return (i.x < j.x);
}

bool SortByYForVector(KeypointEntry i, KeypointEntry j){
  return (i.y < j.y);
}

bool SortBySizeForVector(KeypointEntry i, KeypointEntry j){
  return (i.fsize < j.fsize);
}

bool SortByAngleForVector(KeypointEntry i, KeypointEntry j){
  return (i.angle < j.angle);
}

bool SortByFirstCodewordForVector(KeypointEntry i, KeypointEntry j){
  return (i.codeword_vector[0] < j.codeword_vector[0]);
}


bool SortByXThenYForVector(KeypointEntry i, KeypointEntry j){
  if(i.x < j.x){
    return true;
  }else if(i.x > j.x){
    return false;
  }else{
    if(i.y < j.y){
      return true;
    }else{
      return false;
    }
  }
}

bool SortByXThenYThenSizeForVector(KeypointEntry i, KeypointEntry j){
  if(i.x < j.x){
    return true;
  }else if(i.x > j.x){
    return false;
  }else{
    if(i.y < j.y){
      return true;
    }else if(i.y > j.y){
      return false;
    }else{
      if(i.fsize < j.fsize){
        return true;
      }else{
        return false;
      }
    }
  }
}


bool SortByCodewordForVector(CodewordEntry i, CodewordEntry j){
  return (i.codeword < j.codeword);
}

bool SortByCodewordOnlyForVector(int i, int j){
  return (i < j);
}

void DeltaEncoding::GetDeltaList(std::vector<int>& list){
  std::sort(list.begin(), list.end(), SortByCodewordOnlyForVector);
  for(uint i = list.size() - 1; i >= 1; i--){
    list[i] = list[i] - list[i-1];
  }
}

void DeltaEncoding::Encoding(){
    LoadBinaryDataFromFileForSorting();

    // SortByCoordinateX();
    // getDeltaXCoordinateDistribution();

    // SortByCoordinateXThenY();
    // getDeltaYCoordinateDistribution();

    // SortByCodewordAndDelta();
    // GetDeltaCodewordDistribution();

    // SortByCodewordLength();
    // GetCodewordLengthDeltaDistribution();
    // GetCodewordLengthDistribution();

    // SortByFirstCodewordRawValue();
    // GetFirstRawCodewordDistribution();

    // GetDocument();

    SortByCoordinateXThenYAndDelta();

    // SaveBinaryDataToFile();
}

void DeltaEncoding::GetDocument(){
  int counter = 1;
  for (auto it = keypoint_entry_byname_.cbegin(); it != keypoint_entry_byname_.cend(); it++) {

    std::vector<KeypointEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);

    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
    //   << kps[i].codeword_vector[0] << "," << kps[i].codeword_vector[1] << "," <<kps[i].codeword_vector[2] << " ";
    // }
    // std::cout << "\n";
    const std::string dir = this->DocumentPath + std::to_string(counter) + ".csv";
    FILE * fileDir = fopen(dir.c_str(), "w");
    if( fileDir == NULL){
       std::cout << "Problem! The file: " << dir << " could not be opened!" << std::endl;
       exit(0);
    }
    fprintf(fileDir, "x,y,size,angle,cw1,cw2,cw3\n");
    // for ( uint i = 0; i < delta_x_vector.size(); i++ ){
    //   fprintf(fileDir, "%d\n", delta_x_vector[i]);
    // }
    for ( uint i = 0; i < kps.size(); i++){
      fprintf(fileDir, "%d,%d,%d,%d,%d,%d,%d\n", 
        kps[i].x, kps[i].y, kps[i].fsize, kps[i].angle, kps[i].codeword_vector[0], kps[i].codeword_vector[1], kps[i].codeword_vector[2]);
    }
    fclose(fileDir);
    if(10 == counter++) break;
    }
}

void DeltaEncoding::GetDeltaXCoordinateDistribution(){
  FILE * fileDir = fopen(this->XCoordinateOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->XCoordinateOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "x\n");
  for ( uint i = 0; i < delta_x_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", delta_x_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::GetDeltaYCoordinateDistribution(){
  FILE * fileDir = fopen(this->YCoordinateOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->YCoordinateOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "y\n");
  for ( uint i = 0; i < delta_y_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", delta_y_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::GetDeltaCodewordDistribution(){
  FILE * fileDir = fopen(this->CodewordOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->CodewordOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "codeword\n");
  for ( uint i = 0; i < delta_codeword_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", delta_codeword_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::GetFirstRawCodewordDistribution(){
  FILE * fileDir = fopen(this->CodewordRawValueOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->CodewordRawValueOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "codeword\n");
  for ( uint i = 0; i < codeword_raw_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", codeword_raw_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::GetCodewordLengthDistribution(){
  FILE * fileDir = fopen(this->CodewordLengthOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->CodewordLengthOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "codeword_length\n");
  for ( uint i = 0; i < unique_codeword_length_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", unique_codeword_length_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::GetCodewordLengthDeltaDistribution(){
  FILE * fileDir = fopen(this->DeltaCodewordLengthOutputFile.c_str(), "w");
  if( fileDir == NULL){
     std::cout << "Problem! The file: " << this->DeltaCodewordLengthOutputFile << " could not be opened!" << std::endl;
     exit(0);
  }
  fprintf(fileDir, "codeword_length\n");
  for ( uint i = 0; i < unique_codeword_length_vector.size(); i++ ){
    fprintf(fileDir, "%d\n", unique_codeword_length_vector[i]);
  }
  fclose(fileDir);
}

void DeltaEncoding::SortByCoordinateXAndDelta(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.cbegin(); it != keypoint_entry_byname_.cend(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByXForVector);
    /*take delta list*/
    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i].x << " ";
    // }
    // std::cout << "\n";
    for(uint i = kps.size() - 1; i >= 1; i--){
      kps[i].x = kps[i].x - kps[i-1].x;
    }
    for(uint i = 0; i < kps.size(); i++){
      // std::cout << kps[i].x << " ";
      delta_x_vector.push_back(kps[i].x);
    }
    // std::cout << "\n";
    // if(5 == counter++) break;
  }
}

void DeltaEncoding::SortByCoordinateXThenYAndDelta(){
  int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortByXThenYForVector);
    // std::sort(kps.begin(), kps.end(), SortByYForVector);

    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
    //   << kps[i].codeword_vector[0] << "," << kps[i].codeword_vector[1] << "," <<kps[i].codeword_vector[2] << " ";
    // }

    /*get delta list of the codewords for each keypoint*/
    for(int i = 0; i < kps.size(); i++){
      // std::sort(kps[i].codeword_vector.begin(), kps[i].codeword_vector.end(), SortByCodewordOnlyForVector);
      GetDeltaList(kps[i].codeword_vector);
    }

    for(uint i = 0; i < kps.size(); i++){
      std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
      << kps[i].codeword_vector[0] << "," << kps[i].codeword_vector[1] << "," <<kps[i].codeword_vector[2] << " ";
    }
    std::cout << "\n";
    
    it->second = kps;

    if(1 == ++counter) break;
  }
}

void DeltaEncoding::SortByCodewordLength(){
  // int counter = 0;
  for (auto it = codeword_only_byname_.cbegin(); it != codeword_only_byname_.cend(); it++) {
    std::vector<int> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByCodewordOnlyForVector);

    // std::cout << kps.size() << "\n";
    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i]<< " ";
    // }
    // std::cout << "\n";

    int original_size = kps.size();

    kps.erase( unique( kps.begin(), kps.end() ), kps.end() );

    // std::cout << kps.size() << "\n";
    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i]<< " ";
    // }
    // std::cout << "\n";
    int unique_size = kps.size();

    // unique_codeword_length_vector.push_back(kps.size());
    unique_codeword_length_vector.push_back(original_size - unique_size);
    // if(3 == counter++) break;
  }
}

void DeltaEncoding::SortByCodewordAndDelta(){
  // int counter = 0;
  for (auto it = codeword_only_byname_.cbegin(); it != codeword_only_byname_.cend(); it++) {
    std::vector<int> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByCodewordOnlyForVector);
    /*take delta list*/
    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i]<< " ";
    // }
    // std::cout << "\n";
    for(uint i = kps.size() - 1; i >= 1; i--){
      kps[i] = kps[i] - kps[i-1];
    }
    for(uint i = 0; i < kps.size(); i++){
      // std::cout << kps[i]<< " ";
      delta_codeword_vector.push_back(kps[i]);
    }
    // std::cout << "\n";
    // if(3 == counter++) break;
  }
}

void DeltaEncoding::SortByFirstCodewordRawValue(){
  // int counter = 0;
  for (auto it = codeword_entry_byname_.cbegin(); it != codeword_entry_byname_.cend(); it++) {
    std::vector<CodewordEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByCodewordForVector);
    /*take delta list*/
    // for(uint i = 0; i < kps.size(); i++){
    //   std::cout << kps[i].x << " ";
    // }
    // std::cout << "\n";
    // for(uint i = kps.size() - 1; i >= 1; i--){
    //   kps[i].codeword = kps[i].codeword - kps[i-1].codeword;
    // }
    for(uint i = 0; i < kps.size(); i++){
      // std::cout << kps[i].x << " ";
      codeword_raw_vector.push_back(kps[i].codeword);
    }
    // std::cout << "\n";
    // if(5 == counter++) break;
  }
}

void DeltaEncoding::LoadBinaryDataFromFileForSorting(){
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
        int bytes_read = ParseMultipleCodewordKeypointsForSorting(id, ptr);
        ptr += bytes_read;
      }
    }
    delete[] idbuf;

    fclose(fp);

    std::cout << "total number of bytes in main memory for Forward Index: " << cwkeypoint_byname_.size() * 12 << "\n";
}

int DeltaEncoding::ParseMultipleCodewordKeypointsForSorting(std::string id,
                                                unsigned char *optr) {
    std::vector<CodewordKeyPoint> cwkp_vec;
    auto id_vec_it = cwkeypoint_byname_.find(id);
    if (id_vec_it == cwkeypoint_byname_.end()) cwkeypoint_byname_[id] = cwkp_vec;

    unsigned char *ptr = optr;
    int x = ntohs(*reinterpret_cast<int *>(ptr));
    ptr += 2;
    int y = ntohs(*reinterpret_cast<int *>(ptr));
    ptr += 2;
    int angle = ntohs(*reinterpret_cast<int *>(ptr));
    ptr += 2;
    int fsize = ntohs(*reinterpret_cast<int *>(ptr));
    ptr += 2;
    unsigned char codewordcount = *ptr;
    ptr++;
    std::vector<int> codewords_tmp;
    while (codewordcount > 0) {
      int codeword = *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codewordcount--;
     
      /*original one*/
      // cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
      
      /*the structure for sorting*/
      codewords_tmp.push_back(codeword);

      /*each kp has only 1 cw*/
      // codeword_entry_byname_[id].emplace_back(codeword, x, y, fsize, angle); // for sorting and delta
      
      /*only codeword in here*/
      // codeword_only_byname_[id].emplace_back(codeword);

    }
    keypoint_entry_byname_[id].emplace_back(codewords_tmp, x, y, fsize, angle);
    return static_cast<int>(ptr - optr);
}

void DeltaEncoding::SaveBinaryDataToFileForSorting() {
  int write_version = 1;
  int codeword_count = 3;
  FILE *fp = fopen(this->deltaDocumentFile.c_str(), "wb");

  int fversion = htonl(write_version);
  fwrite(&fversion, 4, 1, fp);

  int fcount = htonl(codeword_count);

  //todo
  for (auto it = cwkeypoint_byname_.cbegin(); it != cwkeypoint_byname_.cend();
       it++) {
    std::string id = it->first;
    int id_bytes = htonl(static_cast<int>(id.size()));
    std::size_t bytes_written = fwrite(&id_bytes, 4, 1, fp);
    bytes_written += fwrite(id.data(), id.size(), 1, fp);

    //write codeword count, assume it is the same number for the one image
    fwrite(&fcount, 4, 1, fp);

    const std::vector<CodewordKeyPoint> &cwkps = it->second;
    std::string data;
    for (auto jj = 0ul; jj < cwkps.size();) {
      // find if following codewords are equivalent
      int repeat = 1;
      std::vector<int> codewords;
      codewords.push_back(cwkps[jj].codeword);
      bool same = true;
      while (jj + repeat < cwkps.size() && same) {
        if (cwkps[jj].EqualKeyPoint(cwkps[jj + repeat])) {
          codewords.push_back(cwkps[jj + repeat].codeword);
          repeat++;
        } else {
          same = false;
        }
      }
      data.append(cwkps[jj].multiBinary(codewords));
      jj += repeat;
    }

    size_t bytes = data.size();
    int nbytes = htonl(static_cast<uint32_t>(bytes));
    fwrite(&nbytes, 4, 1, fp);
    fwrite(data.data(), bytes, 1, fp);
  }
  
  fclose(fp);
  return;
}

// void DeltaEncoding::LoadBinaryDataFromFile(){
//   FILE *fp = fopen(this->binaryDocumentFile.c_str(), "rb");
//   if (!fp){
//     std::cout << this->binaryDocumentFile << " file not found\n";
//   }
//   int fversion = 0;
//     size_t objects_read = fread(&fversion, 4, 1, fp);
//     fversion = ntohl(fversion);
//     if(fversion != 1){
//       std::cerr << "data not version 1: " << fversion << std::endl;
//       fclose(fp);
//     }
//     std::cout << "data version: " << fversion << std::endl;

//     const size_t max_id_length = 1000;
//     char *idbuf = new char[max_id_length];
//     while (!feof(fp)) {

//       uint32_t id_size;
//       size_t b_read = fread(&id_size, 4, 1, fp);
//       if (b_read != 1) {  // this is actually where eof should occur
//         break;
//       }
//       id_size = ntohl(id_size);
//       if (id_size > max_id_length) {
//         // LOG_WARN << "Stopping Reading rerank index, id length of " <<
//         // id_size;
//         break;
//       }
//       objects_read = fread(idbuf, id_size, 1, fp);
//       if (objects_read != 1) {
//         break;
//       }

//       std::string id(idbuf, id_size);
//       size_t nbytes;
//       objects_read = fread(&nbytes, 4, 1, fp);
//       if (objects_read != 1) {
//         break;
//       }
//       nbytes = ntohl(static_cast<uint32_t>(nbytes));
//       if (nbytes > 50000) {
//         // LOG_WARN << "rerank index read giving up, codeword keypoint length of
//         // " << nbytes << " for Id: " << id;
//         break;
//       }
//       std::unique_ptr<unsigned char[]> optr(new unsigned char[nbytes]);
//       unsigned char *ptr = static_cast<unsigned char *>(optr.get());
//       objects_read = fread(ptr, 1, nbytes, fp);
//       if (objects_read < nbytes) {
//         // vs_dict_feat does not have access to qrs LOG_WARN...
//         // LOG_WARN << "rerank index read did not read full data for Id: "
//         // << id;
//         break;
//       }
//       while (ptr + 8 < static_cast<unsigned char *>(optr.get()) + nbytes) {
//         // ptr +8 because absolute minimum number of bytes that will
//         // be read is 9, this way we allow for data that has been padded
//         int bytes_read = ParseMultipleCodewordKeypoints(id, ptr);
//         ptr += bytes_read;
//       }
//     }
//     delete[] idbuf;

//     fclose(fp);

//     std::cout << "total number of bytes in main memory for Forward Index: " << cwkeypoint_byname_.size() * 12 << "\n";
// }

void DeltaEncoding::SaveBinaryDataToFile() {
  int write_version = 1;
  FILE *fp = fopen(this->deltaDocumentFile.c_str(), "wb");
  if (write_version == 0) {
    int fversion = htonl(0);
    fwrite(&fversion, 4, 1, fp);

    for (auto it = cwkeypoint_byname_.cbegin(); it != cwkeypoint_byname_.cend();
         it++) {
      std::string id = it->first;
      int id_bytes = htonl(static_cast<int>(id.size()));
      std::size_t bytes_written = fwrite(&id_bytes, 4, 1, fp);
      bytes_written += fwrite(id.data(), id.size(), 1, fp);

      size_t bytes = it->second.size() * CodewordKeyPoint::bsize0;
      int nbytes = htonl(static_cast<uint32_t>(bytes));
      fwrite(&nbytes, 4, 1, fp);
      void *ptr = malloc(bytes);
      void *origp = ptr;
      for (auto jt = it->second.cbegin(); jt != it->second.cend(); jt++) {
        std::string a = jt->asBinaryV0();
        memcpy(ptr, a.data(), CodewordKeyPoint::bsize0);
        ptr = reinterpret_cast<char *>(ptr) + CodewordKeyPoint::bsize0;
      }
      fwrite(origp, bytes, 1, fp);
      free(origp);
    }
  } else if (write_version == 1) {
    int fversion = htonl(write_version);
    fwrite(&fversion, 4, 1, fp);

    for (auto it = cwkeypoint_byname_.cbegin(); it != cwkeypoint_byname_.cend();
         it++) {
      std::string id = it->first;
      int id_bytes = htonl(static_cast<int>(id.size()));
      std::size_t bytes_written = fwrite(&id_bytes, 4, 1, fp);
      bytes_written += fwrite(id.data(), id.size(), 1, fp);

      const std::vector<CodewordKeyPoint> &cwkps = it->second;
      std::string data;
      for (auto jj = 0ul; jj < cwkps.size();) {
        // find if following codewords are equivalent
        int repeat = 1;
        std::vector<int> codewords;
        codewords.push_back(cwkps[jj].codeword);
        bool same = true;
        while (jj + repeat < cwkps.size() && same) {
          if (cwkps[jj].EqualKeyPoint(cwkps[jj + repeat])) {
            codewords.push_back(cwkps[jj + repeat].codeword);
            repeat++;
          } else {
            same = false;
          }
        }
        data.append(cwkps[jj].multiBinary(codewords));
        jj += repeat;
      }

      size_t bytes = data.size();
      int nbytes = htonl(static_cast<uint32_t>(bytes));
      fwrite(&nbytes, 4, 1, fp);
      fwrite(data.data(), bytes, 1, fp);
    }
  }
  fclose(fp);
  return;
}


// int DeltaEncoding::ParseMultipleCodewordKeypoints(std::string id,
//                                                 unsigned char *optr) {
//     std::vector<CodewordKeyPoint> cwkp_vec;
//     auto id_vec_it = cwkeypoint_byname_.find(id);
//     if (id_vec_it == cwkeypoint_byname_.end()) cwkeypoint_byname_[id] = cwkp_vec;

//     unsigned char *ptr = optr;
//     uint16_t x = ntohs(*reinterpret_cast<int16_t *>(ptr));
//     ptr += 2;
//     uint16_t y = ntohs(*reinterpret_cast<int16_t *>(ptr));
//     ptr += 2;
//     uint16_t angle = ntohs(*reinterpret_cast<int16_t *>(ptr));
//     ptr += 2;
//     uint16_t fsize = ntohs(*reinterpret_cast<int16_t *>(ptr));
//     ptr += 2;
//     unsigned char codewordcount = *ptr;
//     ptr++;
//     while (codewordcount > 0) {
//       unsigned int codeword = *ptr;
//       ptr++;
//       codeword = (codeword << 8) | *ptr;
//       ptr++;
//       codeword = (codeword << 8) | *ptr;
//       ptr++;
//       codewordcount--;
//       cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
//     }

//     return static_cast<int>(ptr - optr);
// }