#include "variable_byte_encoding.h"
#include <netinet/in.h>
#include <memory>
#include <iostream>
#include <assert.h>

template <typename Iterator>
void dumpToFile(const std::string& path, Iterator start, Iterator end) { ///home/qw376/SIGIR2017/data
  // FILE *fdo = fopen64(path.c_str(),"w");
  FILE *fdo = fopen(path.c_str(),"a+");
  if(! fdo) {
    std::cout << "Failed writing to " << path << " (Hint: create folder for path?)" << std::endl;
    return;
  }
  assert(fdo);
  for (; start !=end; ++start)
    if (fwrite(&(*start), sizeof(typename Iterator::value_type), 1, fdo) != 1)
      std::cout << "Failed writing to " << path << " (Hint: create folder for path?)" << std::endl;
  fclose(fdo);
}

int VarByteEncoding::compressionVbytes(std::vector<unsigned uint> input){
   int compressedSize = 0;
   for (int i = 0; i < input.size(); ++i){
          unsigned char byteArr[sizeof(int)];
          bool started = false;
          int x = 0;

          for (x = 0; x < sizeof(int); x++) {
             byteArr[x] = (input[i]%128);
             input[i] /= 128;
          }

          for (x = sizeof(int) - 1; x > 0; x--) {
            if (byteArr[x] != 0 || started == true) {
              started = true;
              byteArr[x] |= 128;
              this->compressedList.push_back(byteArr[x]);
              compressedSize++;
            }
          }
          this->compressedList.push_back(byteArr[0]);
          compressedSize++;
   }
   return compressedSize;
}

int VarByteEncoding::decompressionVbytesInt(unsigned char* input, unsigned * output, int size){
    unsigned char* curr_byte = input;
    unsigned n;
    for (int i = 0; i < size; ++i) {
      unsigned char b = *curr_byte;
      n = b & 0x7F;
//        cout<<"The first byte: "<<n<<endl;
//        print_binary(n);
//        cout<<endl;

      while((b & 0x80) !=0){
        n = n << 7;
        ++curr_byte;
          b = *curr_byte;
          n |= (b & 0x7F);
//          cout<<"The following byte: "<<n<<endl;
//          print_binary(n);
//          cout<<endl;
      }
    ++curr_byte;
    output[i] = n;
  }

  int num_bytes_consumed = (curr_byte - input);
  return (num_bytes_consumed >> 2) + ((num_bytes_consumed & 3) != 0 ? 1 : 0);
}

void VarByteEncoding::loadDataFromFile(){
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
    this->uncompressedList.clear();
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
      int bytes_read = parse_multiple_codeword_keypoints(id, ptr);
      ptr += bytes_read;
    }
    compressionVbytes(this->uncompressedList);
  }
  delete[] idbuf;

  fclose(fp);
}

int VarByteEncoding::parse_multiple_codeword_keypoints(std::string id,
                                                unsigned char *optr) {
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
    this->uncompressedList.push_back(x);
    this->uncompressedList.push_back(y);
    this->uncompressedList.push_back(angle);
    this->uncompressedList.push_back(fsize);
    this->uncompressedList.push_back(codewordcount);

    ptr++;
    while (codewordcount > 0) {
      unsigned int codeword = *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codewordcount--;
      this->uncompressedList.push_back(codeword);
    }

    return static_cast<int>(ptr - optr);
}

void VarByteEncoding::encoding(){
	this->uncompressedList.clear();
	loadDataFromFile();
	dumpToFile(this->varByteDocumentFile, this->compressedList.begin(), this->compressedList.end());
}