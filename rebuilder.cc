#include "rebuilder.h"
#include <netinet/in.h>
#include <memory>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <bitset>
#include "huffman.h"
#include "move_to_front.h"

bool SortByFirstCodewordForVector(KeypointEntry i, KeypointEntry j){
  return (i.codeword_vector[0] < j.codeword_vector[0]);
}

bool SortByAngleForVector(KeypointEntry i, KeypointEntry j){
  return (i.angle < j.angle);
}

bool SortByCodewordOnlyForVector(int i, int j){
  return (i < j);
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

bool SortBySizeThenXThenYForVector(KeypointEntry i, KeypointEntry j){
  if(i.fsize < j.fsize){
    return true;
  }else if(i.fsize > j.fsize){
    return false;
  }else{
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
}

bool SortByLengthThenSymbolCanonicalHuffmanNode(CanonicalHuffmanNode a, CanonicalHuffmanNode b){
  if ( a.length > b.length ){
    return true;
  }else if( a.length < b.length){
    return false;
  }else{
    if(a.symbol < b.symbol){
      return true;
    }else{
      return false;
    }
  }
}

bool UniqueByXThenYThenSizeForVector (KeypointEntry i, KeypointEntry j) {
  return ( (i.x == j.x) && (i.y == j.y) && (i.fsize == j.fsize) );
}

int rebuilder::indexofSmallestElement(int array[], int size){
    int index = 0;

    for(int i = 1; i < size; i++)
    {
        if(array[i] < array[index])
            index = i;              
    }

    return index;
}

int rebuilder::GetPower(int number){
  int power = 0;
  while(number!=0){
    number /= 2;
    power++;
  }
  return power;
}

void rebuilder::RebuildIndex(){
  LoadMapper();
  LoadBinaryDataFromFile();
  DisplayData();
  SortCodewordsWithinKeypoints();
  DisplayData();
  SortByFirstCodewordRawValue();
  DisplayData();
}

void rebuilder::CompressSize(){
  LoadMapper();
  LoadBinaryDataFromFile();
  // SortByFirstCodewordRawValue();
  SortByXThenYThenSize();
  DisplayData();
  // SubstractBaseValueForSize();
  // CaculateActualSize();
}

void rebuilder::VerifyReps(){
  LoadMapper();
  LoadBinaryDataFromFile();
  RemoveRepsOriginalOrder();
}

void rebuilder::RemoveRepsOriginalOrder(){

}

/*huffman on Angle*/
void rebuilder::HuffmanForAngle(){
  LoadMapperAngle();
  LoadBinaryDataFromFile();
  FillBinsHuffmanAngle();
  CaculateHuffmanAngleSize();
}

void rebuilder::LoadMapperAngle(){
  int angle_table_size = 65536;
  angle_table.reserve(angle_table_size);
  for(uint i = 0; i < angle_table.size(); i++){
    angle_table.push_back(0);
  }
  FILE * pFile;
  pFile = fopen (this->angle_remap_file.c_str(), "r");
  int angle_id;
  int freq;
  int position = 0;
  while(fscanf(pFile, "%d %d\n", &angle_id, &freq)!= EOF){
    // std::cout << codeword_id << " " << freq <<"\n";
    angle_table[angle_id] = position++;
  }
  fclose(pFile);
  // std::cout << position << "\n";
}

/*caculate bit average for Angle*/
void rebuilder::CaculateHuffmanAngleSize(){
    long long total_huffman_bits = 0;
    long long total_keypoints = 0;  
    for(auto it = angle_huffman_info_entry_byname_.begin(); it != angle_huffman_info_entry_byname_.end(); it++){
      total_huffman_bits += it->second.total_huffman_code_bits;
      total_keypoints += it->second.image_length;
    }
    std::cout << "total_huffman_bits: " << total_huffman_bits
    <<"\ntotal_16_bits: " << total_keypoints * 16
    <<"\ntotal_keypoints: "<< total_keypoints
    <<"\nbits per number: "<< (double)total_huffman_bits/(double)total_keypoints
    <<"\n";
}

/*caculate bit average for Angle*/
void rebuilder::FillBinsHuffmanAngle(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    /*take out the size value*/
    std::vector<int> angle_values;
    for(uint i = 0; i < kps.size(); i++){
      // angle_values.push_back( 65536 - kps[i].angle );
      // angle_values.push_back( kps[i].angle );
      angle_values.push_back( angle_table[kps[i].angle] );
    }   

    /*take delta for angle*/
    std::sort(angle_values.begin(), angle_values.end());
    for(uint i = angle_values.size() - 1; i >= 1; i--){
      angle_values[i] = angle_values[i] - angle_values[i-1];
    }

    // angle_values.resize(15);
    // for(int i = 0; i < angle_values.size(); i++){
    //   std::cout << angle_values[i] << " ";
    // }
    // std::cout << "\n";
    // moveToFrontApprox(angle_values);
    // for(int i = 0; i < angle_values.size(); i++){
    //   std::cout << angle_values[i] << " ";
    // }
    // std::cout << "\n";
    // angle.mtf_approx_match = approxCopyCounter(cw_values);
    // angle.mtf_exact_match = exactCopyCounter(cw_values);
    // displayComparison(angle_values);
    // moveToFront(angle_values);

    // remapAngle(angle_values);

    AngleHuffmanInfoEntry angle_info;

    /*store image length*/
    angle_info.image_length = angle_values.size();

    /*fill up the bins*/
    for(uint i = 0; i < angle_values.size(); i++){
      for(int j = 1; j < angle_info.number_of_boundries; j++){
        if( (angle_values[i] >= angle_info.boundary[j - 1]) && (angle_values[i] < angle_info.boundary[j]))
          angle_info.bins[j - 1] += 1;
      }
    }

    for(uint i = 0; i < angle_info.number_of_boundries - 1; i++){
      // std::cout << "number less than " << angle_info.boundary[i+1] <<": " << angle_info.bins[i] << "\n";
    }

    /*get huffman code length*/
    Huffman_Canonical(angle_info.bins, angle_info.number_of_boundries - 1, angle_info.huffman_bits_per_bin);

    for(uint i = 0; i < angle_info.number_of_boundries - 1; i++){
      // std::cout << "huffman code length for: " << angle_info.boundary[i+1] <<" is: " << angle_info.huffman_bits_per_bin[i]<< "\n";
    }

    for(uint i = 0; i < angle_info.number_of_boundries - 1; i++){
      uint bits = (angle_info.huffman_bits_per_bin[i] + angle_info.bits_per_bin[i]) * (angle_info.bins[i] );
      angle_info.total_huffman_code_bits += bits;
      // std::cout << "For " << angle_info.boundary[i+1] << " bits(16) is: " << (angle_info.bins[i]) * 16 <<" huffman bits is: " << bits << "\n";
    }

    /*header info*/
    // angle_info.total_huffman_code_bits += 56;//sort by angle, 14*4 = 68

    // angle_info.total_huffman_code_bits += 15;
    if(angle_info.total_huffman_code_bits > angle_values.size() * 16 ){
      angle_info.total_huffman_code_bits = angle_values.size() * 16;
    }
    // angle_info.total_huffman_code_bits += 1;
    // angle_info.total_huffman_code_bits += angle_values.size(); //raw 

    // std::cout << "Image length: " << angle_values.size() << " total bits(16): " << angle_values.size() * 16 << " huffman bits: " << angle_info.total_huffman_code_bits << "\n";

    /*get the actual value of canonical huffman codes*/
    // std::vector<CanonicalHuffmanNode> huffman_code_vector;
    // GetCanonicalHuffmanCode(huffman_code_vector, cw_info.huffman_bits_per_bin, cw_info.number_of_boundries - 1);

    /*put codeword in map*/
    angle_huffman_info_entry_byname_[it->first] = angle_info;

    // if(99 == counter++) break;
  }
}


/*huffman on X*/
void rebuilder::HuffmanForX(){
  // LoadMapper();
  LoadBinaryDataFromFile();
  FillBinsHuffmanX();
  CaculateHuffmanXSize();
}

/*caculate bit average for X*/
void rebuilder::CaculateHuffmanXSize(){
    long long total_huffman_bits = 0;
    long long total_keypoints = 0;  
    long long total_unique_keypoints = 0;
    for(auto it = x_huffman_info_entry_byname_.begin(); it != x_huffman_info_entry_byname_.end(); it++){
      total_huffman_bits += it->second.total_huffman_code_bits;
      total_keypoints += it->second.image_length;
      total_unique_keypoints += it->second.unique_image_length;
    }
    std::cout << "total_huffman_bits: " << total_huffman_bits
    <<"\ntotal_16_bits: " << total_keypoints * 16
    <<"\ntotal_keypoints: "<< total_keypoints
    <<"\ntotal_unique_keypoints: "<< total_unique_keypoints
    <<"\nbits per number: "<< (double)total_huffman_bits/(double)total_keypoints
    <<"\n";
}

/*caculate bit average for X*/
void rebuilder::FillBinsHuffmanX(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    XHuffmanInfoEntry x_info;
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);
    x_info.image_length = kps.size();

    /*sort kps by x,y,size then remove duplicates*/
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    // std::vector<uint16_t> x_values;
    std::vector<int> x_values;
    for(uint i = 0; i < kps.size(); i++){
      x_values.push_back(kps[i].x);
    }   

    // moveToFrontApprox(x_values);

    /*sort by x then take delta on x*/
    // std::sort(x_values.begin(), x_values.end());
    for(uint i = x_values.size() - 1; i >= 1; i--){
      x_values[i] = x_values[i] - x_values[i-1];
    }

    /*move to front coding, transfer number to smaller number*/
    // x_values.resize(25);
    // for(int i = 0; i < x_values.size(); i ++){
    //   std::cout << x_values[i] << " ";
    // }
    // std::cout << "after move to front: \n";
    // moveToFront(x_values);
    // moveToFrontApprox(x_values);
    // for(int i = 0; i < x_values.size(); i ++){
    //   std::cout << x_values[i] << " ";
    // }
    // std::cout << "\n";

    /*store image length*/
    x_info.unique_image_length = x_values.size();

    /*fill up the bins*/
    for(uint i = 0; i < x_values.size(); i++){
      for(int j = 1; j < x_info.number_of_boundries; j++){
        if( (x_values[i] >= x_info.boundary[j - 1]) && (x_values[i] < x_info.boundary[j]))
          x_info.bins[j - 1] += 1;
      }
    }

    for(uint i = 0; i < x_info.number_of_boundries - 1; i++){
      std::cout << "number less than " << x_info.boundary[i+1] <<": " << x_info.bins[i] << "\n";
    }

    /*get huffman code length*/
    Huffman_Canonical(x_info.bins, x_info.number_of_boundries - 1, x_info.huffman_bits_per_bin);

    for(uint i = 0; i < x_info.number_of_boundries - 1; i++){
      std::cout << "huffman code length for: " << x_info.boundary[i+1] <<" is: " << x_info.huffman_bits_per_bin[i]<< "\n";
    }

    for(uint i = 0; i < x_info.number_of_boundries - 1; i++){
      uint bits = (x_info.huffman_bits_per_bin[i] + x_info.bits_per_bin[i]) * (x_info.bins[i] );
      x_info.total_huffman_code_bits += bits;
      std::cout << "For " << x_info.boundary[i+1] << " bits(9) is: " << (x_info.bins[i]) * 9 <<" huffman bits is: " << bits << "\n";
    }

    /*header info*/
    // x_info.total_huffman_code_bits += 30;// sort by cw

    x_info.total_huffman_code_bits += 18;
    x_info.total_huffman_code_bits += x_info.image_length/3;

    // x_info.total_huffman_code_bits += 21;// original order

    std::cout <<"Unique Image length: " << x_info.unique_image_length << " Image length: " << x_info.image_length << " total bits(9): " << x_info.image_length * 9 << " huffman bits: " << x_info.total_huffman_code_bits << "\n";

    /*get the actual value of canonical huffman codes*/
    // std::vector<CanonicalHuffmanNode> huffman_code_vector;
    // GetCanonicalHuffmanCode(huffman_code_vector, cw_info.huffman_bits_per_bin, cw_info.number_of_boundries - 1);

    /*put codeword in map*/
    x_huffman_info_entry_byname_[it->first] = x_info;

    // if(99 == counter++) break;
  }
}

/*huffman on Y*/
void rebuilder::HuffmanForY(){
  LoadMapper();
  LoadBinaryDataFromFile();
  FillBinsHuffmanY();
  CaculateHuffmanYSize();
}

/*caculate bit average for Y*/
void rebuilder::CaculateHuffmanYSize(){
    long long total_huffman_bits = 0;
    long long total_keypoints = 0;  
    long long total_unique_keypoints = 0;
    for(auto it = y_huffman_info_entry_byname_.begin(); it != y_huffman_info_entry_byname_.end(); it++){
      total_huffman_bits += it->second.total_huffman_code_bits;
      total_keypoints += it->second.image_length;
      total_unique_keypoints += it->second.unique_image_length;
    }
    std::cout << "total_huffman_bits: " << total_huffman_bits
    <<"\ntotal_16_bits: " << total_keypoints * 16
    <<"\ntotal_keypoints: "<< total_keypoints
    <<"\ntotal_unique_keypoints: "<< total_unique_keypoints
    <<"\nbits per number: "<< (double)total_huffman_bits/(double)total_keypoints
    <<"\n";
}

/*caculate bit average for Y*/
void rebuilder::FillBinsHuffmanY(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    YHuffmanInfoEntry y_info;
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    y_info.image_length = kps.size();

    /*sort kps by x,y,size then remove duplicates*/
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*sort by cw*/
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*take out the y value*/
    std::vector<int> y_values;
    for(uint i = 0; i < kps.size(); i++){
      y_values.push_back(kps[i].y);
    }   

    /*store image length*/
    y_info.unique_image_length = y_values.size();

    // moveToFrontApprox(y_values);

    /*fill up the bins*/
    for(uint i = 0; i < y_values.size(); i++){
      for(int j = 1; j < y_info.number_of_boundries; j++){
        if( (y_values[i] >= y_info.boundary[j - 1]) && (y_values[i] < y_info.boundary[j]))
          y_info.bins[j - 1] += 1;
      }
    }

    for(uint i = 0; i < y_info.number_of_boundries - 1; i++){
      std::cout << "number less than " << y_info.boundary[i+1] <<": " << y_info.bins[i] << "\n";
    }

    /*get huffman code length*/
    Huffman_Canonical(y_info.bins, y_info.number_of_boundries - 1, y_info.huffman_bits_per_bin);

    for(uint i = 0; i < y_info.number_of_boundries - 1; i++){
      std::cout << "huffman code length for: " << y_info.boundary[i+1] <<" is: " << y_info.huffman_bits_per_bin[i]<< "\n";
    }

    for(uint i = 0; i < y_info.number_of_boundries - 1; i++){
      uint bits = (y_info.huffman_bits_per_bin[i] + y_info.bits_per_bin[i]) * (y_info.bins[i] );
      y_info.total_huffman_code_bits += bits;
      std::cout << "For " << y_info.boundary[i+1] << " bits(9) is: " << (y_info.bins[i]) * 9 <<" huffman bits is: " << bits << "\n";
    }

    /*header info*/
    y_info.total_huffman_code_bits += 15; //sort by xysize
    y_info.total_huffman_code_bits += y_info.image_length/3; //sort by xysize

    // y_info.total_huffman_code_bits += y_info.unique_image_length;
    // y_info.total_huffman_code_bits += 21; //sort by cw

    std::cout <<"Unique Image length: " << y_info.unique_image_length << " Image length: " << y_info.image_length << " total bits(9): " << y_info.image_length * 9 << " huffman bits: " << y_info.total_huffman_code_bits << "\n";

    /*get the actual value of canonical huffman codes*/
    // std::vector<CanonicalHuffmanNode> huffman_code_vector;
    // GetCanonicalHuffmanCode(huffman_code_vector, cw_info.huffman_bits_per_bin, cw_info.number_of_boundries - 1);

    /*put codeword in map*/
    y_huffman_info_entry_byname_[it->first] = y_info;

    // if(99 == counter++) break;
  }
}


/*huffman on scale*/
void rebuilder::HuffmanForScale(){
    LoadMapper();
    LoadBinaryDataFromFile();
    FillBinsHuffmanScale();
    CaculateHuffmanScaleSize();
}

/*caculate bit average for scale*/
void rebuilder::CaculateHuffmanScaleSize(){
    long long total_huffman_bits = 0;
    long long total_keypoints = 0;  
    long long total_unique_keypoints = 0;
    long long total_mtf_approx_match = 0;
    long long total_mtf_exact_match = 0;
    long long number_of_image = scale_huffman_info_entry_byname_.size();
    for(auto it = scale_huffman_info_entry_byname_.begin(); it != scale_huffman_info_entry_byname_.end(); it++){
      total_huffman_bits += it->second.total_huffman_code_bits;
      total_keypoints += it->second.image_length;
      total_unique_keypoints += it->second.unique_image_length;
      total_mtf_approx_match += it->second.mtf_approx_match;
      total_mtf_exact_match += it->second.mtf_exact_match;
    }
    std::cout << "total_huffman_bits: " << total_huffman_bits
    <<"\ntotal_14_bits: " << total_keypoints * 14
    <<"\ntotal_16_bits: " << total_keypoints * 16
    <<"\ntotal_keypoints: "<< total_keypoints
    <<"\ntotal_unique_keypoints: "<< total_unique_keypoints
    <<"\ntotal_mtf_approx_match(include exact): "<< total_mtf_approx_match
    <<"\naverage_total_mtf_approx_match_per_image: " << (double)total_mtf_approx_match/(double)number_of_image
    <<"\ntotal_mtf_exact_match: "<< total_mtf_exact_match
    <<"\naverage_total_mtf_exact_match_per_image: " << (double)total_mtf_exact_match/(double)number_of_image
    <<"\nbits per number: "<< (double)total_huffman_bits/(double)total_keypoints
    <<"\n";
}

/*prescan the data and fill the bins for huffman tree input*/
void rebuilder::FillBinsHuffmanScale(){
    // int counter = 0;
    for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
      ScaleHuffmanInfoEntry scale_info;
      /*take out the keypoint value*/
      std::vector<KeypointEntry> kps = it->second;
      scale_info.image_length = kps.size();

      /*sort kps by x,y,size then remove duplicates*/
      std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
      // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
      kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

      /*sort by codeword*/
      // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

      /*take out the scale value*/
      std::vector<int> scale_values;
      for(uint i = 0; i < kps.size(); i++){
        scale_values.push_back(kps[i].fsize);
      }   


      // std::sort(scale_values.begin(), scale_values.end());
      // for(uint i = scale_values.size() - 1; i >= 1; i--){
      //   scale_values[i] = scale_values[i] - scale_values[i-1];
      // }

      /*figure out boundries*/
      scale_info.min = *std::min_element(scale_values.begin(), scale_values.end());
      scale_info.max = *std::max_element(scale_values.begin(), scale_values.end());
      scale_info.max_minus_min = scale_info.max - scale_info.min;
      // scale_info.max_bit = GetPower(scale_info.max_minus_min);
      for(uint i = 0; i < scale_values.size(); i++){
          scale_values[i] -= scale_info.min;
      }
      /*store image length*/
      scale_info.unique_image_length = scale_values.size();

      // scale_values.resize(25);
      // for(int i = 0; i < scale_values.size(); i ++){
      //   std::cout << scale_values[i] << " ";
      // }
      // std::cout << "\nafter move to front: \n";
      // moveToFront(scale_values);
      // moveToFrontApprox(scale_values);
      // scale_info.mtf_approx_match = approxCopyCounter(scale_values);
      // scale_info.mtf_exact_match = exactCopyCounter(scale_values);
      // std::cout << "size: "<< scale_values.size() << "\n";
      // for(int i = 0; i < scale_values.size(); i ++){
      //   std::cout << scale_values[i] << " ";
      // }
      // std::cout << "\n";

      /*fill up the bins*/
      for(uint i = 0; i < scale_values.size(); i++){
        for(int j = 1; j < scale_info.number_of_boundries; j++){
          if( (scale_values[i] >= scale_info.boundary[j - 1]) && (scale_values[i] < scale_info.boundary[j]))
            scale_info.bins[j - 1] += 1;
        }
      }

      for(uint i = 0; i < scale_info.number_of_boundries - 1; i++){
        std::cout << "number less than " << scale_info.boundary[i+1] <<": " << scale_info.bins[i] << "\n";
      }

      /*get huffman code length*/
      Huffman_Canonical(scale_info.bins, scale_info.number_of_boundries - 1, scale_info.huffman_bits_per_bin);

      for(uint i = 0; i < scale_info.number_of_boundries - 1; i++){
        std::cout << "huffman code length for: " << scale_info.boundary[i+1] <<" is: " << scale_info.huffman_bits_per_bin[i]<< "\n";
      }

      for(uint i = 0; i < scale_info.number_of_boundries - 1; i++){
        uint bits = (scale_info.huffman_bits_per_bin[i] + scale_info.bits_per_bin[i]) * (scale_info.bins[i] );
        scale_info.total_huffman_code_bits += bits;
        std::cout << "For " << scale_info.boundary[i+1] << " bits(14) is: " << (scale_info.bins[i]) * 14 <<" huffman bits is: " << bits << "\n";
      }

      /*header info, 15 length, each length is within 1 - 8, 15 * 4 bits = 60 bits, min takes 14 bits*/
      scale_info.total_huffman_code_bits += 60;
      scale_info.total_huffman_code_bits += 14;
      /*bit stream for duplication*/
      scale_info.total_huffman_code_bits += scale_info.image_length/3;
      // scale_info.total_huffman_code_bits += scale_info.unique_image_length;

      std::cout << "number of approx for MTF(include exact): " << scale_info.mtf_approx_match << "\n";
      std::cout << "number of exact for MTF: " << scale_info.mtf_exact_match << "\n";
      std::cout <<"Unique Image length: " << scale_info.unique_image_length << " Image length: " << scale_info.image_length << " total bits(14): " << scale_values.size() * 14 << " huffman bits: " << scale_info.total_huffman_code_bits << "\n";

      /*get the actual value of canonical huffman codes*/
      // std::vector<CanonicalHuffmanNode> huffman_code_vector;
      // GetCanonicalHuffmanCode(huffman_code_vector, cw_info.huffman_bits_per_bin, cw_info.number_of_boundries - 1);

      /*put codeword in map*/
      scale_huffman_info_entry_byname_[it->first] = scale_info;

      // if(99 == counter++) break;
    }
}


/*get compressed size of huffman coding*/
void rebuilder::HuffmanForCodeword(){
    LoadMapper();
    LoadBinaryDataFromFile();
    FillBinsHuffmanCodeword();
    CaculateHuffmanCWSize();

    /*huffman*/
    // int input[6];
    // input[0] = 2;
    // input[1] = 3;
    // input[2] = 3;
    // input[3] = 5;
    // input[4] = 5;
    // input[5] = 5;
    // std::cout << "start\n";
    // Huffman_Canonical(input, 6);
}

/*caculate bit average for codeword*/
void rebuilder::CaculateHuffmanCWSize(){
    long long total_huffman_bits = 0;
    long long total_keypoints = 0; 
    long long total_mtf_approx_match = 0;
    long long total_mtf_exact_match = 0;
    long long number_of_image = codeword_huffman_info_entry_byname_.size();
    for(auto it = codeword_huffman_info_entry_byname_.begin(); it != codeword_huffman_info_entry_byname_.end(); it++){
      total_huffman_bits += it->second.total_huffman_code_bits;
      total_keypoints += it->second.image_length;
      total_mtf_approx_match += it->second.mtf_approx_match;
      total_mtf_exact_match += it->second.mtf_exact_match;
    }
    std::cout << "total_huffman_bits: " << total_huffman_bits
    <<"\ntotal_21_bits: " << total_keypoints * 21
    <<"\ntotal_24_bits: " << total_keypoints * 24
    <<"\ntotal_keypoints: "<< total_keypoints
    <<"\ntotal_mtf_approx_match(include exact): "<< total_mtf_approx_match
    <<"\naverage_total_mtf_approx_match_per_image: " << (double)total_mtf_approx_match/(double)number_of_image
    <<"\ntotal_mtf_exact_match: "<< total_mtf_exact_match
    <<"\naverage_total_mtf_exact_match_per_image: " << (double)total_mtf_exact_match/(double)number_of_image
    <<"\nbits per number: "<< (double)total_huffman_bits/(double)total_keypoints
    <<"\n";
}

/*prescan the data and fill the bins for huffman tree input*/
void rebuilder::FillBinsHuffmanCodeword(){
    int counter = 0;
    for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
      std::vector<KeypointEntry> kps = it->second;
      // std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);//
      std::sort(kps.begin(), kps.end(), SortByAngleForVector);
      /*take out the keypoint value*/
      std::vector<int> cw_values;
      for(uint i = 0; i < kps.size(); i++){
        cw_values.push_back(kps[i].codeword_vector[0]);
      }      

      /*take delta for codeword*/
      std::sort(cw_values.begin(), cw_values.end());
      for(uint i = cw_values.size() - 1; i >= 1; i--){
        cw_values[i] = cw_values[i] - cw_values[i-1];
      }

      CodewordHuffmanInfoEntry cw_info;

      /*store image length*/
      cw_info.image_length = cw_values.size();

      /*move to front coding, transfer number to smaller number*/
      // cw_values.resize(25);
      // for(int i = 0; i < cw_values.size(); i ++){
      //   std::cout << cw_values[i] << " ";
      // }
      // std::cout << "\nafter move to front: \n";
      // moveToFront(cw_values);
      // moveToFrontApprox(cw_values);

      // cw_info.mtf_approx_match = approxCopyCounter(cw_values);
      // cw_info.mtf_exact_match = exactCopyCounter(cw_values);

      // for(int i = 0; i < cw_values.size(); i ++){
      //   std::cout << cw_values[i] << " ";
      // }
      // std::cout << "\n";
      // displayComparison(cw_values);

      /*fill up the bins*/
      for(uint i = 0; i < cw_values.size(); i++){
        for(int j = 1; j < cw_info.number_of_boundries; j++){
          if( (cw_values[i] >= cw_info.boundary[j - 1]) && (cw_values[i] < cw_info.boundary[j]))
            cw_info.bins[j - 1] += 1;
        }
      }

      // for(uint i = 0; i < cw_info.number_of_boundries - 1; i++){
      //   std::cout << "number less than " << cw_info.boundary[i+1] <<": " << cw_info.bins[i] << "\n";
      // }

      /*get huffman code length*/
      Huffman_Canonical(cw_info.bins, cw_info.number_of_boundries - 1, cw_info.huffman_bits_per_bin);

      // for(uint i = 0; i < cw_info.number_of_boundries - 1; i++){
      //   std::cout << "huffman code length for: " << cw_info.boundary[i+1] <<" is: " << cw_info.huffman_bits_per_bin[i]<< "\n";
      // }

      for(uint i = 0; i < cw_info.number_of_boundries - 1; i++){
        uint bits = (cw_info.huffman_bits_per_bin[i] + cw_info.bits_per_bin[i]) * (cw_info.bins[i] );
        cw_info.total_huffman_code_bits += bits;
        // std::cout << "For " << cw_info.boundary[i+1] << " bits(21) is: " << (cw_info.bins[i]) * 21 <<" huffman bits is: " << bits << "\n";
      }

      /*16 * 4bits = 64 bits*/
      cw_info.total_huffman_code_bits += 64; //raw 
      /*13 * 3bits = 39 bits*/
      // cw_info.total_huffman_code_bits += 72; //delta 18*4 sort by codeword
      // cw_info.total_huffman_code_bits += cw_values.size(); //raw 

      if( cw_info.total_huffman_code_bits > cw_values.size() * 21 ){
          cw_info.total_huffman_code_bits = cw_values.size() * 21;
      }
      cw_info.total_huffman_code_bits += 1; //raw 
      // std::cout << "number of approx for MTF(include exact): " << cw_info.mtf_approx_match << "\n";
      // std::cout << "number of exact for MTF: " << cw_info.mtf_exact_match << "\n";
      // std::cout << "Image length: " << cw_values.size() << " total bits(21): " << cw_values.size() * 21 << " huffman bits: " << cw_info.total_huffman_code_bits << "\n";

      /*get the actual value of canonical huffman codes*/
      // std::vector<CanonicalHuffmanNode> huffman_code_vector;
      // GetCanonicalHuffmanCode(huffman_code_vector, cw_info.huffman_bits_per_bin, cw_info.number_of_boundries - 1);

      /*put codeword in map*/
      codeword_huffman_info_entry_byname_[it->first] = cw_info;

      // if(99 == counter++) break;
    }
}

void rebuilder::GetCanonicalHuffmanCode(std::vector<CanonicalHuffmanNode>& huffman_code_vector, int huffman_bits_per_bin[], int size){
    /*build the huffman node table*/
    for(int i = 0; i < size; i++){
      CanonicalHuffmanNode node(i, huffman_bits_per_bin[i]);
      huffman_code_vector.push_back(node);
    }
    /*sort by length*/
    std::sort(huffman_code_vector.begin(), huffman_code_vector.end(), SortByLengthThenSymbolCanonicalHuffmanNode);

    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << "\n";
    }

    /*init the longest length and value*/
    int length = huffman_code_vector[0].length;
    huffman_code_vector[0].value = 0;

    /*set every value*/
    for(int i = 1; i < huffman_code_vector.size(); i++){
        if(huffman_code_vector[i].length == 0){
          break;
        }
        if(huffman_code_vector[i].length == length){
          huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
        }
        if(huffman_code_vector[i].length < length){
          huffman_code_vector[i].value = huffman_code_vector[i - 1].value + 1;
          int diff = length - huffman_code_vector[i].length; //if length go from 4 to 2, need to shift 2 bits to left
          length = huffman_code_vector[i].length;
          while(diff > 0){
             huffman_code_vector[i].value = huffman_code_vector[i].value  >> 1;
             diff --;
          }
        }
    } 

    /*output all the symbols*/  
    const long unsigned int bit_number = 8;
    for(int i = 0; i < huffman_code_vector.size(); i++){
      std::string binary = std::bitset<bit_number>(huffman_code_vector[i].value).to_string(); //to binary
      std::cout << "huffman code length: " << huffman_code_vector[i].length << " symbol: " << huffman_code_vector[i].symbol << " value: " << huffman_code_vector[i].value << " binary: " << binary << "\n";
    }
}

/*pfd and varbits calculation for size*/
// void rebuilder::CaculateActualSize(){
//   long long total_min_bits = 0;
//   long long total_keypoints = 0;  
//   int var_bits_number = 0;
//   int pfd_number = 0;
//   for(auto it = sizeinfo_entry_byname_.begin(); it != sizeinfo_entry_byname_.end(); it++){
//     // std::cout << "min: " << it->second.min 
//     // << " \nmax: "<< it->second.max 
//     // << " \nmax_minus_min: " << it->second.max_minus_min
//     // << " \nbit1_choice " << it->second.bit1_choice
//     // << " \nbit2_choice " << it->second.bit2_choice
//     // << " \nbit_range " << it->second.bit_range
//     // << " \nimage length " << it->second.image_length
//     // << " \nbit_choice " << it->second.bit_choice_pfd
//     // << " \nbit_range_for_image_length " << it->second.bit_range_for_image_length
//     // << " \ntotal_bits_with_bit1_bit2_choice: " << it->second.minmum_total_bits 
//     // << " \ntotal_bits_with_bit_choice: " << it->second.minmum_total_bits_pfd
//     // << " \ntotal_bits_with_local_bits: " << it->second.total_bits_with_local_bits 
//     // << " \ntotal_bits_with_14_bits: " << it->second.total_bits_with_14_bits 
//     // << " \ntotal_bits_with_16_bits: " << it->second.total_bits_with_16_bits 

//     // << " \nnumbers smaller than 1: " << it->second.statistics[0]
//     // << " \nnumbers smaller than 2: " << it->second.statistics[1]
//     // << " \nnumbers smaller than 4: " << it->second.statistics[2]
//     // << " \nnumbers smaller than 8: " << it->second.statistics[3]
//     // << " \nnumbers smaller than 16: " << it->second.statistics[4]
//     // << " \nnumbers smaller than 32: " << it->second.statistics[5]
//     // << " \nnumbers smaller than 64: " << it->second.statistics[6]
//     // << " \nnumbers smaller than 128: " << it->second.statistics[7]
//     // << " \nnumbers smaller than 256: " << it->second.statistics[8]
//     // << " \nnumbers smaller than 512: " << it->second.statistics[9]
//     // << " \nnumbers smaller than 1024: " << it->second.statistics[10]
//     // << " \nnumbers smaller than 2048: " << it->second.statistics[11]
//     // << " \nnumbers smaller than 4096: " << it->second.statistics[12]
//     // << " \nnumbers smaller than 8192: " << it->second.statistics[13]
//     // << "\n";
//     long long bits = 0;
//     if(it->second.minmum_total_bits < it->second.total_bits_with_local_bits){
//       if(it->second.minmum_total_bits < it->second.minmum_total_bits_pfd){
//         bits = it->second.minmum_total_bits;
//         var_bits_number ++;
//       }else{
//         bits = it->second.minmum_total_bits_pfd;
//         pfd_number ++;
//       }
//     }else{
//       if(it->second.total_bits_with_local_bits < it->second.minmum_total_bits_pfd){
//         bits = it->second.total_bits_with_local_bits;
//         var_bits_number ++;
//       }else{
//         bits = it->second.minmum_total_bits_pfd;
//         pfd_number ++;
//       }
//     }
//     total_min_bits += bits;
//     total_keypoints += it->second.image_length;
//   }

//   std::cout << "total_min_bits: " << total_min_bits
//   <<"\ntotal_14_bits: " << total_keypoints * 14
//   <<"\ntotal_16_bits: " << total_keypoints * 16
//   <<"\nvar_bits_number: " << var_bits_number
//   <<"\npfd_number: " << pfd_number
//   <<"\n";
// }

/*pfd and varbits calculation for size*/
void rebuilder::SubstractBaseValueForSize(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    /*take out the size value*/
    std::vector<uint16_t> size_values;
    for(uint i = 0; i < kps.size(); i++){
      size_values.push_back(kps[i].fsize);
    }

    SizeInfoEntry size_info;
    /*get max, min, max - min, length value, etc*/
    size_info.min = *std::min_element(size_values.begin(), size_values.end());
    size_info.max = *std::max_element(size_values.begin(), size_values.end());
    size_info.max_minus_min = size_info.max - size_info.min;
    /*get total bits according to 2^bits*/
    size_info.bit_range = GetPower(size_info.max_minus_min);
    size_info.image_length = size_values.size();
    size_info.total_bits_with_local_bits = size_info.image_length * size_info.bit_range;
    size_info.total_bits_with_14_bits = size_info.image_length * 14;
    size_info.total_bits_with_16_bits = size_info.image_length * 16;
    size_info.bit_range_for_image_length = GetPower(size_info.image_length);

    for(uint i = 0; i < size_values.size(); i++){
      /*substract the min value*/
      size_values[i] -= size_info.min;

      /*get statistics about value less than 2^bits, accumulation*/
      for(int j = 0; j < 14; j++){
        if( size_values[i] < (1 << j) ){
          size_info.statistics[j] +=1;
        }
      }
    }

    /*get total bits consumption for each power*/
    for(uint i = 0; i < 14; i ++){
      size_info.total_bits_used[i] = size_info.statistics[i] * (i + 1) +  (size_info.image_length - size_info.statistics[i]) * ( size_info.bit_range + 1);
    }

    /*get the minimum bits and its corresponding power*/
    size_info.bit1_choice = indexofSmallestElement(size_info.total_bits_used, 14);
    size_info.bit2_choice = size_info.bit_range - size_info.bit1_choice;

    /*get actual number of bits to compress, 24 means: 10 bits for minmum value, 4 bits for bit1, 4 bits for bit2*/
    size_info.minmum_total_bits = size_info.total_bits_used[size_info.bit1_choice] + 18;

    /*get total bits consumption for each power pfd fashion*/
    for(uint i = 0; i < 14; i ++){
      size_info.total_bits_used_pfd[i] = size_info.statistics[i] * i +  (size_info.image_length - size_info.statistics[i]) * ( size_info.bit_range + size_info.bit_range_for_image_length);
      // std::cout << i << " " << size_info.statistics[i] * (1 << i) << " " << (size_info.image_length - size_info.statistics[i]) * ( size_info.bit_range + size_info.bit_range_for_image_length) << " " << size_info.total_bits_used_pfd[i] << "\n";
    }

    /*get the minimum bits and its corresponding power*/
    size_info.bit_choice_pfd = indexofSmallestElement(size_info.total_bits_used_pfd, 14);

    /*get actual number of bits to compress, 24 means: 10 bits for minmum value, 4 bits for bit choice, 4 bits for image length*/
    size_info.minmum_total_bits_pfd = size_info.total_bits_used_pfd[size_info.bit_choice_pfd] + 18;

    // it->second = kps;

    /*put sizeinfo in map*/
    sizeinfo_entry_byname_[it->first] = size_info;

    // if(9 == counter++) break;
  }
}

void rebuilder::SortByFirstCodewordRawValue(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);
    /*take the delta of the first codeword*/
    // for(uint i = kps.size() - 1; i >= 1; i--){
    //   kps[i].codeword_vector[0] = kps[i].codeword_vector[0] - kps[i-1].codeword_vector[0];
    // }
    it->second = kps;
    // if(0 == counter++) break;
  }
}

void rebuilder::SortByXThenYThenSize(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    /*take the delta of the first codeword*/
    // for(uint i = kps.size() - 1; i >= 1; i--){
    //   kps[i].x = kps[i].x - kps[i-1].x;
    // }
    it->second = kps;
    // if(0 == counter++) break;
  }
}

void rebuilder::SortCodewordsWithinKeypoints(){
  // int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    /*take the delta of the first codeword*/
    for(uint i = 0; i < kps.size(); i++){
       std::sort(kps[i].codeword_vector.begin(), kps[i].codeword_vector.end(), SortByCodewordOnlyForVector);
       for(uint j = kps[i].codeword_vector.size() - 1; j >=1; j--){
          kps[i].codeword_vector[j] = kps[i].codeword_vector[j] - kps[i].codeword_vector[j-1];
       }
    }
    it->second = kps;
    // if(0 == counter++) break;
  }
}

void rebuilder::DisplayData(){
  int counter = 0;
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    std::vector<KeypointEntry> kps = it->second;
    std::vector<uint16_t> size_values;
    for(uint i = 0; i < kps.size(); i++){
      std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
      << kps[i].codeword_vector[0] << "\n";
      // std::cout << kps[i].x << "," << kps[i].y << "," << kps[i].fsize << "," << kps[i].angle << "," 
      // << kps[i].codeword_vector[0] << "," << kps[i].codeword_vector[1] << "," <<kps[i].codeword_vector[2] << "\n";
      // std::cout << kps[i].codeword_vector[0] << " ";
      // std::cout << kps[i].codeword_vector[0] << "," << kps[i].codeword_vector[1] << "," <<kps[i].codeword_vector[2] << " ";
      // std::cout << kps[i].fsize << "\n";
      // size_values.push_back(kps[i].fsize);
    }
    // std::sort(size_values.begin(), size_values.end());
    // int min = *std::min_element(size_values.begin(), size_values.end());
    // for(uint i = 0; i < size_values.size(); i++){
    //   std::cout << size_values[i] -  min << "\n";
    // }
    std::cout << "\nsize of the document: " << kps.size();
    std::cout << "\n-----end-of-document-----\n";
    // if(999 == counter++) break;
  }
}

void rebuilder::LoadMapper(){
	int codeword_table_size = 2000000;
	order_table.reserve(codeword_table_size);
	for(uint i = 0; i < order_table.size(); i++){
		order_table.push_back(0);
	}

	FILE * pFile;
	pFile = fopen (this->table_file.c_str(), "r");
	int codeword_id;
	int freq;
	int position = 0;
	while(fscanf(pFile, "%d %d\n", &codeword_id, &freq)!= EOF){
		// std::cout << codeword_id << " " << freq <<"\n";
		order_table[codeword_id] = position++;
	}
	fclose(pFile);
	// std::cout << position << "\n";
}

void rebuilder::LoadBinaryDataFromFile(){
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
    // std::cout << "data version: " << fversion << std::endl;

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
      // std::cout << file_count << "\n";
      std::cout << file_count++ << "\n";
      // if( file_count > 500000){
      if( file_count > 999){
      // if( file_count > 0){
      // if( file_count > 9){
      	break;
      }

    }
    delete[] idbuf;

    fclose(fp);

    // std::cout << "total number of images in main memory for Forward Index: " << file_count << "\n";
}


int rebuilder::ParseMultipleCodewordKeypoints(std::string id,
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
    ptr++;
    std::vector<int> codewords_tmp;
    while (codewordcount > 0) {
      unsigned int codeword = *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      // std::cout << x << "," << y << "," << fsize << "," << angle << "," << (int)codewordcount << "," <<codeword << " ";
      codewordcount--;
      codewords_tmp.push_back(codeword);
      // codewords_tmp.push_back(order_table[codeword]);
      // cwkeypoint_byname_[id].emplace_back(codeword, x, y, fsize, angle); // duplicate information stored here, x, y, fsize, angle, comment by me
    }
    keypoint_entry_byname_[id].emplace_back(codewords_tmp, x, y, fsize, angle);

    return static_cast<int>(ptr - optr);
}

void rebuilder::Vbyte(){
  num_vbyte_vars = 0;
  std::cout << "new Vbyte\n";
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*sort kps by x,y,size then remove duplicates*/
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    // std::vector<uint16_t> x_values;
    std::vector<unsigned int> values;
    for(uint i = 0; i < kps.size(); i++){
      values.push_back(kps[i].fsize);
      num_vbyte_vars++;
    }

    // for(uint i = values.size() - 1; i >= 1; i--){
    //   values[i] = values[i] - values[i-1];
    // }   

    compressionVbytes(values);
  }

  std::cout << "vbyte uncompressed size: " << num_vbyte_vars*2 << "\n";
  std::cout << "vbyte compressed size: " << this->compressedList.size() << "\n";
}

int rebuilder::compressionVbytes(std::vector<unsigned uint> input){
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

void rebuilder::pfd(){
  
  //threshold initalization
  pfdb[0] = 2;
  pfdb[1] = 4;
  pfdb[2] = 8;
  pfdb[3] = 16;
  pfdb[4] = 32;
  pfdb[5] = 64;
  pfdb[6] = 128;
  pfdb[7] = 256;
  pfdb[8] = 512;
  pfdb[9] = 1024;
  pfdb[10] = 2048;
  pfdb[11] = 4096;
  pfdb[12] = 8192;
  pfdb[13] = 16384;
  pfdb[14] = 32768;
  pfdb[15] = 65536;
  pfdb[16] = 131072;
  pfdb[17] = 262144;
  pfdb[18] = 524288;
  pfdb[19] = 1048576;
  pfdb[20] = 2097152;

  pfd_bits = 0;

  num_vbyte_vars = 0;
  std::cout << "pfd\n";
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*sort kps by x,y,size then remove duplicates*/
    // std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    // kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    // std::vector<uint16_t> x_values;
    std::vector<unsigned int> values;
    for(uint i = 0; i < kps.size(); i++){
      values.push_back(kps[i].codeword_vector[0]);
      num_vbyte_vars++;
    }

    /*only for x*/
    // for(uint i = values.size() - 1; i >= 1; i--){
    //   values[i] = values[i] - values[i-1];
    // }   

    //get number of chunks and the remainder
    int num_chunks = values.size()/32;
    int remainder = values.size()%32; 

    std::cout << "\nnew image: " << "\n";
    std::cout << "image length: " << values.size() << "\n";
    std::cout << "num of chunks: " << num_chunks << " remainder: " << remainder << std::endl;
    // pfd_bits += remainder * x_bits;
    // pfd_bits += remainder * y_bits;
    // pfd_bits += remainder * size_bits;
    // pfd_bits += remainder * angle_bits;
    pfd_bits += remainder * cw_bits;

    //scan chunks to get proper b and number of exceptions for each chunk
    for(int i=0; i<num_chunks; i++) {
      int b = 0;
      int count = 0;
      std::cout << "chunk num: " << i+1 << "\n";
      for(int k = 0; k <= 20; k++) {
        for(int j = 32*i; j < 32*(i+1); j++) {
          if(values[j] < pfdb[k]){
            count ++;
          }
        }

        if(count >= threshold) {
          b = k+1;
          std::cout << "threshold: " << pfdb[k] << "\n";
          std::cout << "b: " << b << "\n";
          std::cout << "number of vars below threshold: " << count << "\n";
          std::cout << "# of exceptions: " << 32 - count << "\n";

          // pfd_bits += 33*b + (32-count)*x_bits;
          // pfd_bits += 33*b + (32-count)*y_bits;
          // pfd_bits += 33*b + (32-count)*size_bits;
          // pfd_bits += 33*b + (32-count)*angle_bits;
          pfd_bits += 33*b + (32-count)*cw_bits;
          break;
        }else{
          count = 0;
        }
      }
    }
  
  }

  std::cout << "uncompressed size: " << num_vbyte_vars*24 << "\n";
  std::cout << "compressed size: " <<  pfd_bits << "\n";
}

void rebuilder::golomb(){
  
  //threshold initalization
  golomb_bits = 0;

  num_vbyte_vars = 0;
  std::cout << "golomb\n";
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*sort kps by x,y,size then remove duplicates*/
    // std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    // kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    // std::vector<uint16_t> x_values;
    std::vector<unsigned int> values;
    for(uint i = 0; i < kps.size(); i++){
      // values.push_back(kps[i].codeword_vector[0]);
      values.push_back(kps[i].angle);
      // values.push_back(kps[i].y);
      // values.push_back(kps[i].x);
      num_vbyte_vars++;
    }

    /*only for x*/
    // for(uint i = values.size() - 1; i >= 1; i--){
    //   values[i] = values[i] - values[i-1];
    // }   

    /*calculating ave*/
    int sum = 0;
    for(uint i = 0; i < values.size(); i++){
      sum += values[i];
    }
    float ave = sum/values.size();
    float b = a * ave;
    std::cout << "sum: " << sum << "\n";
    std::cout << "ave: " << ave << "\n";
    std::cout << "b param: " << b << "\n"; 

    int b_int = b;

    if(b_int!=0){
      int binary_bits = 0;
      while(b_int) {
        b_int/=2;
        binary_bits++;
      }
      std::cout << "binary_bits: " << binary_bits << "\n";

      for(uint i = 0; i < values.size(); i++){
        int unary = values[i]/(int)b;
        // int binary = values[i]%(int)b;
        // std::cout << unary << " " << binary << "\n";
        golomb_bits += (unary+1);
        golomb_bits += binary_bits;
      }
    } else {
       for(uint i = 0; i < values.size(); i++){
        golomb_bits += (values[i]+1);
      }
    }
  }

  // std::cout << "uncompressed size: " << num_vbyte_vars*24 << "\n";
  std::cout << "uncompressed size: " << num_vbyte_vars*16 << "\n";
  std::cout << "compressed size: " <<  golomb_bits << "\n";
}

void rebuilder::simple9() {
  //threshold initalization
  simple9_bits = 0;

  num_vbyte_vars = 0;
  std::cout << "simple9\n";
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*sort kps by x,y,size then remove duplicates*/
    std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    std::vector<unsigned int> values;
    for(uint i = 0; i < kps.size(); i++){
      // values.push_back(kps[i].codeword_vector[0]);
      // values.push_back(kps[i].angle);
      // values.push_back(kps[i].fsize);
      // values.push_back(kps[i].y);
      values.push_back(kps[i].x);
      num_vbyte_vars++;
    }

    /*only for x*/
    for(uint i = values.size() - 1; i >= 1; i--){
      values[i] = values[i] - values[i-1];
    }

    std::vector<unsigned int> bits;
    for(uint i = 0; i < values.size(); i++) {
      int binary_bits = 0;
      int v = values[i];
      while(v){
       v= v/2;
       binary_bits++;
      }
      if(values[i] == 0) {
        binary_bits = 1;
      }
      // std::cout << values[i] << " " << binary_bits << "\n";
      bits.push_back(binary_bits);
    }

    /*for test*/
    // for(uint i=0; i<bits.size(); i++){
    //   std::cout << bits[i] << " ";
    // }
    // std::cout << "\n";

    for(uint i = 0; i < bits.size(); i++) {
      /*check if next 28 numbers fits into 1 bits each*/
      if(i+28 <= bits.size()){//if we have 28 numbers
        uint j;
        for(j = i; j < i+28; j++) {
            if(bits[j]!=1){
              break;
            }
        }
        if(j == i+28) { //if fits go seal those 28 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "28 1's ";
          i+=27;
          continue;
        }
      }

      /*check if next 14 numbers fits into 2 bits each*/
      if(i+14 <= bits.size()){//if we have 14 numbers
        uint j;
        for(j = i; j < i+14; j++) {
            if(bits[j]>2){
              break;
            }
        }
        if(j == i+14) { //if fits go seal those 14 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "14 2's ";
          i+=13;
          continue;
        }
      }

      /*check if next 9 numbers fits into 3 bits each*/
      if(i+9 <= bits.size()){//if we have 9 numbers
        uint j;
        for(j = i; j < i+9; j++) {
            if(bits[j]>3){
              break;
            }
        }
        if(j == i+9) { //if fits go seal those 9 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "9 3's ";
          i+=8;
          continue;
        }
      }

      /*check if next 7 numbers fits into 4 bits each*/
      if(i+7 <= bits.size()){//if we have 7 numbers
        uint j;
        for(j = i; j < i+7; j++) {
            if(bits[j]>4){
              break;
            }
        }
        if(j == i+7) { //if fits go seal those 7 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "7 4's ";
          i+=6;
          continue;
        }
      }

      /*check if next 5 numbers fits into 5 bits each*/
      if(i+5 <= bits.size()){//if we have 5 numbers
        uint j;
        for(j = i; j < i+5; j++) {
            if(bits[j]>5){
              break;
            }
        }
        if(j == i+5) { //if fits go seal those 5 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "5 5's ";
          i+=4;
          continue;
        }
      }

      /*check if next 4 numbers fits into 7 bits each*/
      if(i+4 <= bits.size()){//if we have 4 numbers
        uint j;
        for(j = i; j < i+4; j++) {
            if(bits[j]>7){
              break;
            }
        }
        if(j == i+5) { //if fits go seal those 5 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "4 7's ";
          i+=3;
          continue;
        }
      }

      /*check if next 3 numbers fits into 9 bits each*/
      if(i+3 <= bits.size()){//if we have 3 numbers
        uint j;
        for(j = i; j < i+3; j++) {
            if(bits[j]>9){
              break;
            }
        }
        if(j == i+3) { //if fits go seal those 5 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "3 9's ";
          i+=2;
          continue;
        }
      }

      /*check if next 2 numbers fits into 14 bits each*/
      if(i+2 <= bits.size()){//if we have 2 numbers
        uint j;
        for(j = i; j < i+2; j++) {
            if(bits[j]>14){
              break;
            }
        }
        if(j == i+2) { //if fits go seal those 5 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "2 14's ";
          i+=1;
          continue;
        }
      }

      /*check if next 1 number fits into 28 bits each*/
      if(i+1 <= bits.size()){//if we have 1 numbers
        uint j;
        for(j = i; j < i+1; j++) {
            if(bits[j]>28){
              break;
            }
        }
        if(j == i+1) { //if fits go seal those 5 numbers
          simple9_bits+=28;
          simple9_bits+=4;
          // std::cout << "1 28's ";
          i+=0;
          continue;
        }
      }

    }
      // std::cout << "\n";
  }
  // std::cout << "uncompressed size: " << num_vbyte_vars*24 << "\n";
  std::cout << "uncompressed size: " << num_vbyte_vars*16 << "\n";
  std::cout << "compressed size: " <<  simple9_bits << "\n";
  
}

void rebuilder::gzip(){
  
  //threshold initalization
  golomb_bits = 0;

  num_vbyte_vars = 0;
  std::cout << "gzip\n";
  std::ofstream outfile;
  outfile.open("junk.dat", std::ios::binary | std::ios::out);
  LoadBinaryDataFromFile();
  for (auto it = keypoint_entry_byname_.begin(); it != keypoint_entry_byname_.end(); it++) {
    /*take out the keypoint value*/
    std::vector<KeypointEntry> kps = it->second;
    // std::sort(kps.begin(), kps.end(), SortByFirstCodewordForVector);

    /*sort kps by x,y,size then remove duplicates*/
    // std::sort(kps.begin(), kps.end(), SortByXThenYThenSizeForVector);
    // std::sort(kps.begin(), kps.end(), SortBySizeThenXThenYForVector);
    // kps.erase( unique( kps.begin(), kps.end(), UniqueByXThenYThenSizeForVector), kps.end() );

    /*take out the x value*/
    // std::vector<uint16_t> x_values;
    std::vector<unsigned int> values;
    for(uint i = 0; i < kps.size(); i++){
      values.push_back(kps[i].codeword_vector[0]);
      // values.push_back(kps[i].angle);
      // values.push_back(kps[i].fsize);
      // values.push_back(kps[i].y);
      // values.push_back(kps[i].x);
      num_vbyte_vars++;
    }

    /*only for x*/
    // for(uint i = values.size() - 1; i >= 1; i--){
    //   values[i] = values[i] - values[i-1];
    // }   

    /*for x, y, size, angle*/
    // for(uint i = 0; i < values.size(); i++) {
    //   short tmp = values[i];
    //   outfile.write((char*)&tmp, sizeof(short)); // sizeof can take a type
    // }

    /*for cw*/
    for(uint i = 0; i < values.size(); i++) {
      int v = values[i];
      char c1 = v >> 16;
      outfile.write(&c1, sizeof(char)); // sizeof can take a type
      char c2 = (v >> 8) & 0b00000000000000000000000011111111;
      outfile.write(&c2, sizeof(char)); // sizeof can take a type
      char c3 = (v & 0b00000000000000000000000011111111);
      outfile.write(&c3, sizeof(char)); // sizeof can take a type
      // std::cout << v << " " << (short)c1 << " " << (short)c2 << " " << (short)(c3) << " " << std::endl;
      // std::cout << std::bitset<32>(v) << " " << std::bitset<8>(c1) << " " << std::bitset<8>(c2) << " " << std::bitset<8>(c3) << " " << std::endl;
    }

  }
  outfile.close();

  /*test*/
  // std::ifstream infile; 
  // infile.open("junk.dat", std::ios::binary | std::ios::in);
  //   for(uint i = 0; i < 10; i++){
  //   short t;
  //   infile.read((char*)&t, sizeof(short)); // reads 2 bytes into a cell that is either 2 or 4
  //   std::cout << t << std::endl;
  // }
  // infile.close();
 
  // std::cout << "uncompressed size: " << num_vbyte_vars*24 << "\n";
  std::cout << "uncompressed size: " << num_vbyte_vars << "\n";
  std::cout << "compressed size: " <<  num_vbyte_vars << "\n";
}

#define KB *(1<<10)
#define MB *(1<<20)
#define BUFFERSIZE ((1 MB) - 1)
#define FUZ_NB_TESTS  (128 KB)
#define PROBATABLESIZE (4 KB)
#define FUZ_UPDATERATE  200
#define PRIME1   2654435761U
#define PRIME2   2246822519U

void rebuilder::ans_test(){
  std::cout << "ans\n";
  size_t sourceSize = 10;
  size_t dstSize = FSE_compressBound(sourceSize);

  BYTE* bufferSource   = (BYTE*) malloc (sourceSize);
  BYTE* bufferDst   = (BYTE*) malloc (BUFFERSIZE+64);
  BYTE* bufferVerif = (BYTE*) malloc (BUFFERSIZE+64);
  bufferDst[dstSize] = {0};

  bufferSource[0] = 1;
  bufferSource[1] = 2;
  bufferSource[2] = 1;
  bufferSource[3] = 2;
  bufferSource[4] = 2;
  bufferSource[5] = 1;
  bufferSource[6] = 2;
  bufferSource[7] = 3;
  bufferSource[8] = 2;
  bufferSource[9] = 2;


  size_t sizeCompressed = FSE_compress (bufferDst, dstSize, bufferSource, sourceSize);
  std::cout << "\nCompressed function return " << sizeCompressed << "\n";
  std::cout << "compression error name: " << FSE_getErrorName(sizeCompressed) << "\n";

  std::cout << "\nCompressed Content:\n";
  for(uint i = 0; i < 10; i++) {
    std::cout << (int)bufferDst[i] << "\n";
    std::cout << std::bitset<8>(bufferDst[i]) << "\n";
  }

  size_t d_return = FSE_decompress(bufferVerif, 10, bufferDst, sizeCompressed);
  std::cout << "\nDecompressed function return " << d_return << "\n";
  std::cout << "decompression error name: " << FSE_getErrorName(d_return) << "\n";
  // FSE_decompress(void* dst,  size_t dstCapacity, const void* cSrc, size_t cSrcSize);

  std::cout << "\nDecompressed Content:\n";
  for(uint i = 0; i < 10; i++) {
    std::cout << (int)bufferVerif[i] << "\n";
    std::cout << std::bitset<8>(bufferVerif[i]) << "\n";
  }
}