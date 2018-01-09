/**
 *  Created on: June 7th, 2017
 *      Author: Qi Wang
 *      Copyright 2017 Blippar
 */

#ifndef __ENTROPY_H_
#define __ENTROPY_H_

/** Max number of bytes (i.e., tokens) */
#define LIBDO_MAX_BYTES      2000000

#include <string>
#include <vector>

class Entropy {
 public:
  Entropy();
  /** 
   * Given a pointer to an array of bytes, return a float indicating the
   * level of entropy in bits (a number between zero and eight),
   * assuming a space of 256 possible byte values. The second argument
   * indicates the number of bytes in the sequence. If this sequence
   * runs into unallocated memory, this function should fail with a
   * SIGSEGV.
   */
  float    shannon_test(uint32_t*, const int&);

  /** Report the number of (unique) tokens seen. This is _not_ the
      number of individual events seen. For example, if the library sees
      the string `aaab', the number of events is 4 and the number of
      tokens is 2. */
  int      get_num_tokens(void){ return m_num_tokens; };

  /** Returns maximum entropy for byte distributions log2(256)=8 bits*/
  float    get_max_entropy(void){ return m_maxent; };

  /** Returns the ratio of entropy to maxentropy */
  float    get_entropy_ratio(void){ return m_ratio; };

  /*get the data entropy*/
  float   shannon_entropy_from_file();
 
 /*image wise entropy average*/
  void cw_avrage_entropy();
  void get_cw_entropy_dis();

 private:
  /** Frequecies for each byte */
  // int m_token_freqs[LIBDO_MAX_BYTES]; //frequency of each token in sample
  // float m_token_probs[LIBDO_MAX_BYTES]; //P(each token appearing)
  uint32_t* m_token_freqs; //frequency of each token in sample
  float* m_token_probs; //P(each token appearing)
  int m_num_tokens; //actual number of `seen' tokens, max 256 
  float m_maxent;
  float m_ratio;
  int LIBDISORDER_INITIALIZED;
  uint64_t num_events;
  std::vector<float> entropy_vector;

  void initialize_lib();
  void count_num_tokens();
  void get_token_frequencies(uint32_t* buf, const int& length);
  void get_token_frequencies();
  Entropy (const Entropy&);
  Entropy& operator= (const Entropy&);

  const std::string binaryDocumentFile = "/home/qw376/fic_data/rerank.bin";

  int parse_multiple_codeword_keypoints(std::string id,
                                      unsigned char *optr);

  const std::string cw_entropy_output = "/home/qw376/fic_data/csv_files/cw_entropy.csv";

};
#endif // Entropy