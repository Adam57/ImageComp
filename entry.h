#ifndef ENTRY_H
#define ENTRY_H
#include <vector>

class CanonicalHuffmanNode{
public:
	int symbol,
	length,
	value;
	CanonicalHuffmanNode(int s, int l){
		symbol = s;
		length = l;
		value = 0;
	}
};

#define ANGLE_MAX_BITS 16
#define ANGLE_SIZE 17
// #define ANGLE_SIZE 8
// #define ANGLE_SIZE 5
class AngleHuffmanInfoEntry{
public:
	uint32_t
	image_length,
	total_huffman_code_bits;

	/*cluster */
	// int bins[15];
	// int boundary[16];
	// int huffman_bits_per_bin[15];
	// int bits_per_bin[15];
	// uint number_of_boundries;

	/*classic for bespoken data*/
	int bins[ANGLE_SIZE];
	int boundary[ANGLE_SIZE + 1];
	int huffman_bits_per_bin[ANGLE_SIZE];
	int bits_per_bin[ANGLE_SIZE];
	uint number_of_boundries;

	AngleHuffmanInfoEntry(){
		total_huffman_code_bits = 0;


		number_of_boundries = ANGLE_SIZE + 1;

		/*sort by angle*/
		// boundary[0] = 0; // 0
		// boundary[1] = 1; // 2^0
		// boundary[2] = 2; // 2^1
		// boundary[3] = 4;  // 2^2
		// boundary[4] = 8; // 2^3
		// boundary[5] = 16; //2^4
		// boundary[6] = 32; //2^5
		// boundary[7] = 64; //2^6
		// boundary[8] = 128; //2^7
		// boundary[9] = 256; // 2^8
		// boundary[10] = 512; // 2^9
		// boundary[11] = 1024; // 2^10
		// boundary[12] = 2048; // 2^11
		// boundary[13] = 4096; // 2^12
		// boundary[14] = 65536; // 2^16

		// bits_per_bin[0] = 0;
		// bits_per_bin[1] = 0;
		// bits_per_bin[2] = 1;
		// bits_per_bin[3] = 2;
		// bits_per_bin[4] = 3;
		// bits_per_bin[5] = 4;
		// bits_per_bin[6] = 5;
		// bits_per_bin[7] = 6;
		// bits_per_bin[8] = 7;
		// bits_per_bin[9] = 8;
		// bits_per_bin[10] = 9;
		// bits_per_bin[11] = 10;
		// bits_per_bin[12] = 11;
		// bits_per_bin[13] = 12;
		// bits_per_bin[14] = 16;

		// /*bins for Angle, classic*/
		boundary[0] = 0; // 0
		boundary[1] = 1; // 2^0
		boundary[2] = 2; // 2^1
		boundary[3] = 4;  // 2^2
		boundary[4] = 8; // 2^3
		boundary[5] = 16; //2^4
		boundary[6] = 32; //2^5
		boundary[7] = 64; //2^6
		boundary[8] = 128; //2^7
		boundary[9] = 256; // 2^8
		boundary[10] = 512; // 2^9
		boundary[11] = 1024; // 2^10
		boundary[12] = 2048; // 2^11
		boundary[13] = 4096; // 2^12
		boundary[14] = 8192; // 2^13
		boundary[15] = 16384; // 2^14
		boundary[16] = 32768; // 2^15
		// boundary[17] = 65536; // 2^16
		boundary[17] = 65536 * 2; // 2^18


		/*we can deduct the boundary except the first one*/
		bits_per_bin[0] = 0;
		bits_per_bin[1] = 0;
		bits_per_bin[2] = 1;
		bits_per_bin[3] = 2;
		bits_per_bin[4] = 3;
		bits_per_bin[5] = 4;
		bits_per_bin[6] = 5;
		bits_per_bin[7] = 6;
		bits_per_bin[8] = 7;
		bits_per_bin[9] = 8;
		bits_per_bin[10] = 9;
		bits_per_bin[11] = 10;
		bits_per_bin[12] = 11;
		bits_per_bin[13] = 12;
		bits_per_bin[14] = 13;
		bits_per_bin[15] = 14;
		// bits_per_bin[16] = 16;
		bits_per_bin[16] = 17;


		/*bins for Angle*/
		// boundary[0] = 0; // 0
		// boundary[1] = 2048; //11
		// boundary[2] = 4096; //11
		// boundary[3] = 8192; //12
		// boundary[4] = 16384; //13
		// boundary[5] = 20480;  //12
		// boundary[6] = 47000; //15
		// boundary[7] = 51096; // 12
		// boundary[8] = 65536; //14

		// bits_per_bin[0] = 11;
		// bits_per_bin[1] = 11;
		// bits_per_bin[2] = 12;
		// bits_per_bin[3] = 13;
		// bits_per_bin[4] = 12;
		// bits_per_bin[5] = 15;
		// bits_per_bin[6] = 12;
		// bits_per_bin[7] = 14;


		// boundary[0] = 0; // 0
		// boundary[1] = 16383; //13
		// boundary[2] = 20478;  //12
		// boundary[3] = 47000; //15
		// boundary[4] = 51095; // 12
		// boundary[5] = 65536; //14

		// bits_per_bin[0] = 13;
		// bits_per_bin[1] = 12;
		// bits_per_bin[2] = 15;
		// bits_per_bin[3] = 12;
		// bits_per_bin[4] = 14;

		// boundary[0] = 0; // 0
		// boundary[1] = 16300; //14
		// boundary[2] = 18347;  //11
		// boundary[3] = 48200; //15
		// boundary[4] = 50247; // 11
		// boundary[5] = 65536; //14

		// bits_per_bin[0] = 14;
		// bits_per_bin[1] = 11;
		// bits_per_bin[2] = 15;
		// bits_per_bin[3] = 11;
		// bits_per_bin[4] = 14;




		/*initialize bins*/
		for(uint i = 0; i < number_of_boundries - 1; i++){
			// bins[i] = 1; // every bin normalize by starting with 1, so that we don't lose intervals in the huffman process
			bins[i] = 0;
		}
	}

};

#define SCALE_SIZE 15 //sort by cw/xysize
#define SCALE_BITS_IN_HEADER 4
class ScaleHuffmanInfoEntry{
public:
	uint32_t 
	mtf_approx_match,
	mtf_exact_match,
	max_bit,
	min,
	max,
	max_minus_min,
	image_length,
	unique_image_length,
	total_huffman_code_bits;

	/*raw codeword*/
	// int bins[15];
	// int boundary[16];
	// int huffman_bits_per_bin[15];
	// int bits_per_bin[15];
	// uint number_of_boundries;

	/*delta codeword for bespoken data*/
	int bins[SCALE_SIZE];
	int boundary[SCALE_SIZE + 1];
	int huffman_bits_per_bin[SCALE_SIZE];
	int bits_per_bin[SCALE_SIZE];
	uint number_of_boundries;

	ScaleHuffmanInfoEntry(){
		total_huffman_code_bits = 0;

		number_of_boundries = SCALE_SIZE + 1;


		/*sort by cw/xysize*/
		boundary[0] = 0; // 0
		boundary[1] = 1; // 2^0
		boundary[2] = 2; // 2^1
		boundary[3] = 4;  // 2^2
		boundary[4] = 8; // 2^3
		boundary[5] = 16; //2^4
		boundary[6] = 32; //2^5
		boundary[7] = 64; //2^6
		boundary[8] = 128; // 2^7
		boundary[9] = 256; // 2^8
		boundary[10] = 512; // 2^9
		boundary[11] = 1024; // 2^10
		boundary[12] = 2048; // 2^11
		boundary[13] = 4096; // 2^12
		boundary[14] = 8192; // 2^13
		boundary[15] = 16384; // 2^14

		bits_per_bin[0] = 0;
		bits_per_bin[1] = 0;
		bits_per_bin[2] = 1;
		bits_per_bin[3] = 2;
		bits_per_bin[4] = 3;
		bits_per_bin[5] = 4;
		bits_per_bin[6] = 5;
		bits_per_bin[7] = 6;
		bits_per_bin[8] = 7;
		bits_per_bin[9] = 8;
		bits_per_bin[10] = 9;
		bits_per_bin[11] = 10;
		bits_per_bin[12] = 11;
		bits_per_bin[13] = 12;
		bits_per_bin[14] = 13;


		/*scale for original order + mtf*/
		// boundary[0] = 0; // 0
		// boundary[1] = 1; // 2^0
		// boundary[2] = 2; // 2^1
		// boundary[3] = 4;  // 2^2
		// boundary[4] = 8; // 2^3
		// boundary[5] = 16; //2^4
		// boundary[6] = 32; //2^5
		// boundary[7] = 64; //2^6
		// boundary[8] = 128; // 2^7
		// boundary[9] = 256; // 2^8
		// boundary[10] = 512; // 2^9
		// boundary[11] = 1024; // 2^10
		// boundary[12] = 2048; // 2^11
		// boundary[13] = 4096; // 2^12
		// boundary[14] = 8192; // 2^13
		// boundary[15] = 16384 * 2; // 2^14

		// /*we can deduct the boundary except the first one*/
		// bits_per_bin[0] = 0;
		// bits_per_bin[1] = 0;
		// bits_per_bin[2] = 1;
		// bits_per_bin[3] = 2;
		// bits_per_bin[4] = 3;
		// bits_per_bin[5] = 4;
		// bits_per_bin[6] = 5;
		// bits_per_bin[7] = 6;
		// bits_per_bin[8] = 7;
		// bits_per_bin[9] = 8;
		// bits_per_bin[10] = 9;
		// bits_per_bin[11] = 10;
		// bits_per_bin[12] = 11;
		// bits_per_bin[13] = 12;
		// bits_per_bin[14] = 13 + 1;

		/*initialize bins*/
		for(uint i = 0; i < number_of_boundries - 1; i++){
			// bins[i] = 1; // every bin normalize by starting with 1, so that we don't lose intervals in the huffman process
			bins[i] = 0;
		}
	}
};

#define X_BITS_IN_HEADER 3
#define X_SIZE 6 // sort by x then y then size
// #define X_SIZE 7 // original order
// #define X_SIZE  10//sort by codeword
class XHuffmanInfoEntry{
public:
	uint32_t
	image_length,
	unique_image_length,
	total_huffman_code_bits;

	/*raw codeword*/
	// int bins[15];
	// int boundary[16];
	// int huffman_bits_per_bin[15];
	// int bits_per_bin[15];
	// uint number_of_boundries;

	/*delta codeword for bespoken data*/
	int bins[X_SIZE];
	int boundary[X_SIZE + 1];
	int huffman_bits_per_bin[X_SIZE];
	int bits_per_bin[X_SIZE];
	uint number_of_boundries;

	XHuffmanInfoEntry(){
		total_huffman_code_bits = 0;

		/*bins for X*/
		number_of_boundries = X_SIZE + 1;

		/*sort by cw*/
		/*draw boundaries here*/
		// boundary[0] = 0; // 0
		// boundary[1] = 8;  // 2^3
		// boundary[2] = 16; // 2^4
		// boundary[3] = 32; //2^5
		// boundary[4] = 64; //2^6
		// boundary[5] = 128; //2^7
		// boundary[6] = 256; // 2^8
		// boundary[7] = 512; // 2^9


		// /*we can deduct the boundary except the first one*/
		// bits_per_bin[0] = 3;
		// bits_per_bin[1] = 3;
		// bits_per_bin[2] = 4;
		// bits_per_bin[3] = 5;
		// bits_per_bin[4] = 6;
		// bits_per_bin[5] = 7;
		// bits_per_bin[6] = 8;

		/*sort by x then y then size*/
		boundary[0] = 0; // 0
		boundary[1] = 1; // 2^0
		boundary[2] = 2; // 2^1
		boundary[3] = 4; // 2^2
		boundary[4] = 8;  // 2^3
		boundary[5] = 16; // 2^4
		boundary[6] = 512; // 2^9

		/*we can deduct the boundary except the first one*/
		bits_per_bin[0] = 0;
		bits_per_bin[1] = 0;
		bits_per_bin[2] = 1;
		bits_per_bin[3] = 2;
		bits_per_bin[4] = 3;
		bits_per_bin[5] = 9;

		/*original order + unique*/
		// boundary[0] = 0; // 0
		// // boundary[1] = 1; // 2^0
		// // boundary[2] = 2; // 2^1
		// // boundary[3] = 4; // 2^2
		// boundary[1] = 8;  // 2^3
		// boundary[2] = 16; // 2^4
		// boundary[3] = 32; //2^5
		// boundary[4] = 64; //2^6
		// boundary[5] = 128; //2^7
		// boundary[6] = 256; // 2^8
		// boundary[7] = 512; // 2^9

		// /*we can deduct the boundary except the first one*/
		// bits_per_bin[0] = 3;
		// bits_per_bin[1] = 3;
		// bits_per_bin[2] = 4;
		// bits_per_bin[3] = 5;
		// bits_per_bin[4] = 6;
		// bits_per_bin[5] = 7;
		// bits_per_bin[6] = 8;


		/*original order + unique + MTF*/
		// boundary[0] = 0; // 0
		// // boundary[1] = 1; // 2^0
		// // boundary[2] = 2; // 2^1
		// // boundary[3] = 4; // 2^2
		// boundary[1] = 8;  // 2^3
		// boundary[2] = 16; // 2^4
		// boundary[3] = 32; //2^5
		// boundary[4] = 64; //2^6
		// boundary[5] = 128; //2^7
		// boundary[6] = 256; // 2^8
		// boundary[7] = 512 * 2; // 2^9

		// /*we can deduct the boundary except the first one*/
		// bits_per_bin[0] = 3;
		// bits_per_bin[1] = 3;
		// bits_per_bin[2] = 4;
		// bits_per_bin[3] = 5;
		// bits_per_bin[4] = 6;
		// bits_per_bin[5] = 7;
		// bits_per_bin[6] = 10;



		/*initialize bins*/
		for(uint i = 0; i < number_of_boundries - 1; i++){
			// bins[i] = 1; // every bin normalize by starting with 1, so that we don't lose intervals in the huffman process
			bins[i] = 0;
		}
	}

};

// #define Y_SIZE 7 //sort by cw
#define Y_SIZE 5 //sort by x then y then size
#define Y_BITS_IN_HEADER 3
class YHuffmanInfoEntry{
public:
	uint32_t
	image_length,
	unique_image_length,
	total_huffman_code_bits;

	/*raw codeword*/
	// int bins[15];
	// int boundary[16];
	// int huffman_bits_per_bin[15];
	// int bits_per_bin[15];
	// uint number_of_boundries;

	/*delta codeword for bespoken data*/
	int bins[Y_SIZE];
	int boundary[Y_SIZE + 1];
	int huffman_bits_per_bin[Y_SIZE];
	int bits_per_bin[Y_SIZE];
	uint number_of_boundries;

	YHuffmanInfoEntry(){
		total_huffman_code_bits = 0;

		/*bins for Y*/
		number_of_boundries = Y_SIZE + 1;


		/*sort by x then y then size*/
		boundary[0] = 0; // 0
		boundary[1] = 32; //2^5
		boundary[2] = 64; //2^6
		boundary[3] = 128; //2^7
		boundary[4] = 256; // 2^8
		boundary[5] = 512; // 2^9

		bits_per_bin[0] = 5;
		bits_per_bin[1] = 5;
		bits_per_bin[2] = 6;
		bits_per_bin[3] = 7;
		bits_per_bin[4] = 8;

		/*sort by cw*/
		// boundary[0] = 0; // 0
		// boundary[1] = 8;  // 2^3
		// boundary[2] = 16; // 2^4
		// boundary[3] = 32; //2^5
		// boundary[4] = 64; //2^6
		// boundary[5] = 128; //2^7
		// boundary[6] = 256; // 2^8
		// boundary[7] = 512; // 2^9

		// bits_per_bin[0] = 3;
		// bits_per_bin[1] = 3;
		// bits_per_bin[2] = 4;
		// bits_per_bin[3] = 5;
		// bits_per_bin[4] = 6;
		// bits_per_bin[5] = 7;
		// bits_per_bin[6] = 8;


		/*initialize bins*/
		for(uint i = 0; i < number_of_boundries - 1; i++){
			// bins[i] = 1; // every bin normalize by starting with 1, so that we don't lose intervals in the huffman process
			bins[i] = 0;
		}
	}

};

#define CW_MAX_BITS 21
#define CW_SIZE 16
// #define CW_SIZE 18 //delta codeoword, sort by codeword
class CodewordHuffmanInfoEntry{
public:
	uint32_t
	mtf_approx_match,
	mtf_exact_match,
	image_length,
	total_huffman_code_bits;

	/*raw codeword*/
	// int bins[15];
	// int boundary[16];
	// int huffman_bits_per_bin[15];
	// int bits_per_bin[15];
	// uint number_of_boundries;

	/*delta codeword for bespoken data*/
	uint number_of_boundries;
	int bins[CW_SIZE];
	int boundary[CW_SIZE + 1];
	int huffman_bits_per_bin[CW_SIZE];
	int bits_per_bin[CW_SIZE];

	CodewordHuffmanInfoEntry(){
		total_huffman_code_bits = 0;
		number_of_boundries = CW_SIZE + 1;

		/*bins for raw cw*/
		boundary[0] = 0; //start
		boundary[1] = 64; // 2^7
		boundary[2] = 128; // 2^7
		boundary[3] = 256; // 2^8
		boundary[4] = 512; // 2^9
		boundary[5] = 1024; // 2^10
		boundary[6] = 2048; // 2^11
		boundary[7] = 4096; // 2^12
		boundary[8] = 8192; // 2^13
		boundary[9] = 16384; // 2^14
		boundary[10] = 32768; // 2^15
		boundary[11] = 65536; // 2^16
		boundary[12] = 131072; // 2^17
		boundary[13] = 262144; // 2^18
		boundary[14] = 524288; // 2^19
		boundary[15] = 1048576; // 2^20
		// boundary[16] = 2097152; // 2^21
		boundary[16] = 2097152 * 2; // 2^21

		bits_per_bin[0] = 6;
		bits_per_bin[1] = 6;
		bits_per_bin[2] = 7;
		bits_per_bin[3] = 8;
		bits_per_bin[4] = 9;
		bits_per_bin[5] = 10;
		bits_per_bin[6] = 11;
		bits_per_bin[7] = 12;
		bits_per_bin[8] = 13;
		bits_per_bin[9] = 14;
		bits_per_bin[10] = 15;
		bits_per_bin[11] = 16;
		bits_per_bin[12] = 17;
		bits_per_bin[13] = 18;
		bits_per_bin[14] = 19;
		// bits_per_bin[15] = 20;
		bits_per_bin[15] = 22;

		/*bins for delta cw, sort by codeword*/
		// /*draw boundaries here*/
		// boundary[0] = 0; // 0
		// boundary[1] = 1; // 2^0
		// boundary[2] = 2; // 2^1
		// boundary[3] = 4;  // 2^2
		// boundary[4] = 8; // 2^3
		// boundary[5] = 16; //2^4
		// boundary[6] = 32; //2^5
		// boundary[7] = 64; //2^6
		// boundary[8] = 128; // 2^7
		// boundary[9] = 256; // 2^8
		// boundary[10] = 512; // 2^9
		// boundary[11] = 1024; // 2^10
		// boundary[12] = 2048; // 2^11
		// boundary[13] = 4096; // 2^12
		// boundary[14] = 8192; // 2^13
		// boundary[15] = 16384; // 2^14
		// boundary[16] = 32768; // 2^15
		// boundary[17] = 65536; // 2^16
		// // boundary[18] = 131072; // 2^17
		// // boundary[19] = 262144; // 2^18
		// // boundary[20] = 524288; //2^19
		// // boundary[21] = 1048576; //2^20
		// boundary[18] = 2097152; // 2^21

		// /*we can deduct the boundary except the first one*/
		// bits_per_bin[0] = 0;
		// bits_per_bin[1] = 0;
		// bits_per_bin[2] = 1;
		// bits_per_bin[3] = 2;
		// bits_per_bin[4] = 3;
		// bits_per_bin[5] = 4;
		// bits_per_bin[6] = 5;
		// bits_per_bin[7] = 6;
		// bits_per_bin[8] = 7;
		// bits_per_bin[9] = 8;
		// bits_per_bin[10] = 9;
		// bits_per_bin[11] = 10;
		// bits_per_bin[12] = 11;
		// bits_per_bin[13] = 12;
		// bits_per_bin[14] = 13;
		// bits_per_bin[15] = 14;
		// bits_per_bin[16] = 15;
		// // bits_per_bin[17] = 16;
		// // bits_per_bin[18] = 17;
		// // bits_per_bin[19] = 18;
		// // bits_per_bin[20] = 19;
		// bits_per_bin[17] = 21;


		/*initialize bins*/
		for(uint i = 0; i < number_of_boundries - 1; i++){
			// bins[i] = 1; // every bin normalize by starting with 1, so that we don't lose intervals in the huffman process
			bins[i] = 0;
		}
	}

};

class SizeInfoEntry{
public:
	uint16_t min,
	max,
	max_minus_min,
	number_zeros,
	bit1_choice,
	bit2_choice,
	bit_choice_pfd,
	bit_range,
	bit_range_for_image_length,
	image_length;
	uint16_t statistics[14]; // number smaller than 1 bits, 2 bits, etc
	int total_bits_used[14]; // with 1 bits, 2 bits, choice, etc
	int total_bits_used_pfd[14]; // pfd fashion compression
	int minmum_total_bits,
	minmum_total_bits_pfd,
	total_bits_with_local_bits,
	total_bits_with_14_bits,
	total_bits_with_16_bits;
	SizeInfoEntry(){
		for(uint i = 0; i < 14; i++){
			statistics[i] = 0;
			total_bits_used[i] = 0;
			total_bits_used_pfd[i] = 0;
		}
	}
};

class KeypointEntry{
public:
	std::vector<int> codeword_vector;
	int x;
	int y;
	int fsize;  // fixed point representation (size * 128)
	int angle;  // fixed point representation
	// int concat_x_y;
	// int shuffle_x_y;
	KeypointEntry(std::vector<int> cw1, int x1, int y1, int sf1, int a1);
	KeypointEntry(){}
	// KeypointEntry(KeypointEntry kp2){
	// 	codeword_vector = kp2.codeword_vector;
	// 	x = kp2.x;
	// 	y = kp2.y;
	// 	fsize = kp2.fsize;
	// 	angle = kp2.angle;
	// }
};

inline KeypointEntry::KeypointEntry(std::vector<int> cw1, int x1, int y1, int sf1, int a1) :
                     codeword_vector(cw1), x(x1), y(y1), fsize(sf1), angle(a1) {}

class CodewordEntry{
public:
	int codeword;
	int x;
	int y;
	int fsize;  // fixed point representation (size * 128)
	int angle;  // fixed point representation
	// int concat_x_y;
	// int shuffle_x_y;
	CodewordEntry(int cw1, int x1, int y1, int sf1, int a1);
	CodewordEntry(){}
};

inline CodewordEntry::CodewordEntry(int cw1, int x1, int y1, int sf1, int a1) :
                     codeword(cw1), x(x1), y(y1), fsize(sf1), angle(a1) {}

class CodewordIndexEntry{
public:
  int codeword;
  uint32_t count;
  CodewordIndexEntry(int cw, int cnt);
};

inline CodewordIndexEntry::CodewordIndexEntry(int cw, int cnt) : codeword(cw), count(cnt){}

#endif