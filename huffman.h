/*************************************************************************
* Name:        huffman.h
* Author:      Qi Wang
* Description: Huffman coder/decoder interface.
* Copyright:   2017 Blippar
*-------------------------------------------------------------------------
*************************************************************************/

#ifndef _huffman_h_
#define _huffman_h_

#ifdef __cplusplus
extern "C" {
#endif


/*************************************************************************
* Function prototypes
*************************************************************************/

int Huffman_Compress( unsigned char *in, unsigned char *out,
                      unsigned int insize );
void Huffman_Uncompress( unsigned char *in, unsigned char *out,
                         unsigned int insize, unsigned int outsize );

/*************************************************************************
* Types used for Huffman coding
*************************************************************************/

typedef struct {
    unsigned char *BytePtr;
    unsigned int  BitPos;
} huff_bitstream_t;

typedef struct {
    int Symbol;
    unsigned int Count;
    unsigned int Code;
    unsigned int Bits;
} huff_sym_t;

typedef struct huff_encodenode_struct huff_encodenode_t;

struct huff_encodenode_struct {
    huff_encodenode_t *ChildA, *ChildB;
    int Count;
    int Symbol;
};

typedef struct huff_decodenode_struct huff_decodenode_t;

struct huff_decodenode_struct {
    huff_decodenode_t *ChildA, *ChildB;
    int Symbol;
};


/*************************************************************************
* Constants for Huffman decoding
*************************************************************************/

/* The maximum number of nodes in the Huffman tree is 2^(8+1)-1 = 511 */
#define MAX_TREE_NODES 511
// #define MAX_TREE_NODES 16

/*************************************************************************
* Functions for Huffman Canonical Coding
*************************************************************************/
void Huffman_Canonical(int input[], unsigned int insize, int output[]);


#ifdef __cplusplus
}
#endif

#endif /* _huffman_h_ */