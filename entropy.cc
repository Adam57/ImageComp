/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */

#include <math.h> //for log2()
#include <stdio.h> //for NULL
#include "entropy.h"
#include <iostream>
#include <memory>
#include <stdint.h>
#include <netinet/in.h>
#include <cstdint>

#if defined(__FreeBSD__)
#define        log2(x) (log((x)) * (1./M_LN2))//M_LN2 in math.h 0.69314718055994530942
#endif

Entropy::Entropy()
: m_num_tokens(0)
, m_maxent(0.0)
, m_ratio(0.0)
, LIBDISORDER_INITIALIZED(0)
{
}

/** Frequecies for each byte */
//static int m_token_freqs[LIBDO_MAX_BYTES]; //frequency of each token in sample
//static float m_token_probs[LIBDO_MAX_BYTES]; //P(each token appearing)
//static int m_num_tokens = 0; //actual number of `seen' tokens, max 256 
//static float m_maxent = 0.0;
//static float m_ratio = 0.0;
//static int LIBDISORDER_INITIALIZED = 0;

void
Entropy::initialize_lib()
{
  int i = 0;
  if(1 == LIBDISORDER_INITIALIZED)
    return;

  m_num_tokens = 0;

  m_token_freqs = new uint32_t[LIBDO_MAX_BYTES];
  m_token_probs = new float[LIBDO_MAX_BYTES];

  for(i = 0; i < LIBDO_MAX_BYTES; i++)
  {
    m_token_freqs[i]=0;
    m_token_probs[i]=0.0;
  }

  LIBDISORDER_INITIALIZED = 1;
}

/**
 * Set m_num_tokens by iterating over m_token_freq[] and maintaining
 * a counter of each position that does not hold the value of zero.
 */
void
Entropy::count_num_tokens()
{
  int i = 0;
  int counter = 0;
  for(i = 0; i < LIBDO_MAX_BYTES; i++)
  {
    if(0 != m_token_freqs[i])
    {
      counter++;
    }
  }
  m_num_tokens = counter;
  return;
}

/**
 * Sum frequencies for each token (i.e., byte values 0 through 255)
 * We assume the `length' parameter is correct.
 *
 * This function is available only to functions in this file.
 */
void
Entropy::get_token_frequencies(uint32_t* buf, 
	     	               const int& length)
{
  int i=0;
  uint32_t* itr=NULL;
  uint32_t c=0;

  itr = buf;

  //reset number of tokens
  m_num_tokens = 0;

  //make sure freqency and probability arrays are cleared
  for(i = 0; i < LIBDO_MAX_BYTES; i++)
  {
    m_token_freqs[i] = 0;
    m_token_probs[i] = 0.0;
  }

  for( i = 0; i < length; i++)
  {
    c = (uint32_t)*itr;
    std::cout <<"symbol: " << c << "\n";
    //assert(0<=c<LIBDO_MAX_BYTES);
    m_token_freqs[c]++;
    itr++;
  }
}

/**
 * Return entropy (in bits) of this buffer of ints. We assume that the
 * `length' parameter is correct. 
 */
float
Entropy::shannon_test(uint32_t* buf, 
             const int& length)
{
  int i = 0;
  float bits = 0.0;
  uint32_t* itr = NULL; //values of itr should be zero to 255
  uint32_t token;
  float freq = 0.0; //loop variable for holding freq from m_token_freq[]
  float entropy = 0.0; //running entropy sum
  num_events = 0; //`length' parameter

  if(NULL == buf || 0 == length)
    return 0.0;

  if(0 == LIBDISORDER_INITIALIZED)
    initialize_lib();

  itr = buf;
  m_maxent = 0.0;
  m_ratio = 0.0;
  num_events = length;
  get_token_frequencies(itr, num_events); //modifies m_token_freqs[]
  //set m_num_tokens by counting unique m_token_freqs entries
  count_num_tokens(); 

  if(m_num_tokens > LIBDO_MAX_BYTES)
  {
    //report error somehow?
    return 0.0;
  }

  //iterate through whole m_token_freq array, but only count
  //spots that have a registered token (i.e., freq>0)
  for(i = 0; i < LIBDO_MAX_BYTES; i++)
  {
    if(0!=m_token_freqs[i])
    {
      token = i;
      freq = ((float)m_token_freqs[token]); 
      m_token_probs[token] = (freq / ((float)num_events)); //num_events is the input buffer length
      entropy += m_token_probs[token] * log2(m_token_probs[token]);
    }
  }

  bits = -1.0 * entropy;
  // m_maxent = log2(m_num_tokens); //m_num_tokens is the unique number of symbols
  m_ratio = bits / m_maxent;


  std::cout << "Total occurrence of integers " << num_events << "\n";
  std::cout << "Distinct number of integers: " << m_num_tokens << "\n";
  // std::cout << "m_maxent: " << m_maxent << "\n";
  std::cout << "Entropy: " << bits << "\n";

  return bits;
}

/**
 * Return entropy (in bits) of this buffer of bytes while loading the documents
 */

float
Entropy::shannon_entropy_from_file(){

  int i = 0;
  float bits = 0.0;
  float freq = 0.0; //loop variable for holding freq from m_token_freq[]
  float entropy = 0.0; //running entropy sum
  num_events = 0; //`length' parameter
  uint32_t token;

  if(0 == LIBDISORDER_INITIALIZED){
    initialize_lib();
  }

  m_maxent = 0.0;
  m_ratio = 0.0;

  //got token frequencies from file
  get_token_frequencies();

  count_num_tokens(); 


  if(m_num_tokens > LIBDO_MAX_BYTES)
  {
    //report error somehow?
    return 0.0;
  }

  //iterate through whole m_token_freq array, but only count
  //spots that have a registered token (i.e., freq>0)
  for(i = 0; i < LIBDO_MAX_BYTES; i++)
  {
    if(0 != m_token_freqs[i])
    {
      token = i;
      freq = ((float)m_token_freqs[token]); 
      m_token_probs[token] = (freq / ((float)num_events)); //num_events is the input buffer length
      entropy += m_token_probs[token] * log2(m_token_probs[token]);
    }
  }

  bits = -1.0 * entropy;
  // m_maxent = log2(m_num_tokens); //m_num_tokens is the unique number of symbols
  m_ratio = bits / m_maxent;

  std::cout << "Total occurrence of integers " << num_events << "\n";
  std::cout << "Distinct number of integers: " << m_num_tokens << "\n";
  // std::cout << "m_maxent: " << m_maxent << "\n";
  std::cout << "Entropy: " << bits << "\n";
  // std::cout << "bits per symbol: " << m_ratio << "\n";

  return bits;
}

void
Entropy::get_token_frequencies(){

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
  }
  delete[] idbuf;

  fclose(fp);
}

int 
Entropy::parse_multiple_codeword_keypoints(std::string id,
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

    // m_token_freqs[x]++;
    // m_token_freqs[y]++;
    // m_token_freqs[angle]++;
    // m_token_freqs[fsize]++;
    m_token_freqs[codewordcount]++; // this can be ingored
    // num_events += 5;
    // num_events += 4;
    num_events ++;
    ptr++;
    while (codewordcount > 0) {
      unsigned int codeword = *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codeword = (codeword << 8) | *ptr;
      ptr++;
      codewordcount--;
      // m_token_freqs[codeword]++;
      // num_events++;
    }

    return static_cast<int>(ptr - optr);
}