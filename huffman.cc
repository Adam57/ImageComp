/*************************************************************************
* Name:        huffman.cc
* Author:      Qi Wang
* Copyright:   2017 Blippar
* Description: Huffman coder/decoder implementation.
*
* This is a very straight forward implementation of a Huffman coder and
* decoder.
*
* Primary flaws with this primitive implementation are:
*  - Slow bit stream implementation
*  - Maximum tree depth of 32 (the coder aborts if any code exceeds a
*    size of 32 bits). If I'm not mistaking, this should not be possible
*    unless the input buffer is larger than 2^32 bytes, which is not
*    supported by the coder anyway (max 2^32-1 bytes can be specified with
*    an unsigned 32-bit integer).
*
* On the other hand, there are a few advantages of this implementation:
*  - The Huffman tree is stored in a very compact form, requiring only
*    10 bits per symbol (for 8 bit symbols), meaning a maximum of 320
*    bytes overhead.
*  - The code should be fairly easy to follow, if you are familiar with
*    how the Huffman compression algorithm works.
*
* Possible improvements (probably not worth it):
*  - Partition the input data stream into blocks, where each block has
*    its own Huffman tree. With variable block sizes, it should be
*    possible to find locally optimal Huffman trees, which in turn could
*    reduce the total size.
*  - Allow for a few different predefined Huffman trees, which could
*    reduce the size of a block even further.
*-------------------------------------------------------------------------
*************************************************************************/

#include "huffman.h"
#include <iostream>

/*************************************************************************
*                           INTERNAL FUNCTIONS                           *
*************************************************************************/


/*************************************************************************
* _Huffman_InitBitstream() - Initialize a bitstream.
*************************************************************************/

static void _Huffman_InitBitstream( huff_bitstream_t *stream,
    unsigned char *buf )
{
  stream->BytePtr  = buf;
  stream->BitPos   = 0;
}


/*************************************************************************
* _Huffman_ReadBit() - Read one bit from a bitstream.
*************************************************************************/

static unsigned int _Huffman_ReadBit( huff_bitstream_t *stream )
{
  unsigned int  x, bit;
  unsigned char *buf;

  /* Get current stream state */
  buf = stream->BytePtr;
  bit = stream->BitPos;

  /* Extract bit */
  x = (*buf & (1<<(7-bit))) ? 1 : 0;
  bit = (bit+1) & 7;
  if( !bit )
  {
    ++ buf;
  }

  /* Store new stream state */
  stream->BitPos = bit;
  stream->BytePtr = buf;

  return x;
}


/*************************************************************************
* _Huffman_Read8Bits() - Read eight bits from a bitstream.
*************************************************************************/

static unsigned int _Huffman_Read8Bits( huff_bitstream_t *stream )
{
  unsigned int  x, bit;
  unsigned char *buf;

  /* Get current stream state */
  buf = stream->BytePtr;
  bit = stream->BitPos;

  /* Extract byte */
  x = (*buf << bit) | (buf[1] >> (8-bit));
  ++ buf;

  /* Store new stream state */
  stream->BytePtr = buf;

  return x;
}


/*************************************************************************
* _Huffman_WriteBits() - Write bits to a bitstream.
*************************************************************************/

static void _Huffman_WriteBits( huff_bitstream_t *stream, unsigned int x,
  unsigned int bits )
{
  unsigned int  bit, count;
  unsigned char *buf;
  unsigned int  mask;

  /* Get current stream state */
  buf = stream->BytePtr;
  bit = stream->BitPos;

  /* Append bits */
  mask = 1 << (bits-1);
  for( count = 0; count < bits; ++ count )
  {
    *buf = (*buf & (0xff^(1<<(7-bit)))) +
            ((x & mask ? 1 : 0) << (7-bit));
    x <<= 1;
    bit = (bit+1) & 7;
    if( !bit )
    {
      ++ buf;
    }
  }

  /* Store new stream state */
  stream->BytePtr = buf;
  stream->BitPos  = bit;
}


/*************************************************************************
* _Huffman_Hist() - Calculate (sorted) histogram for a block of data.
*************************************************************************/

static void _Huffman_Hist( unsigned char *in, huff_sym_t *sym,
  unsigned int size )
{
  int k;

  /* Clear/init histogram */
  for( k = 0; k < 256; ++ k )
  {
    sym[k].Symbol = k;
    sym[k].Count  = 0;
    sym[k].Code   = 0;
    sym[k].Bits   = 0;
  }

  /* Build histogram */
  for( k = size; k; -- k )
  {
    sym[*in++].Count ++;
  }
}

/*************************************************************************
* _Huffman_StoreTree() - Store a Huffman tree in the output stream and
* in a look-up-table (a symbol array).
*************************************************************************/

static void Huffman_Canonical_StoreTree( huff_encodenode_t *node, huff_sym_t *sym,
  huff_bitstream_t *stream, unsigned int code, unsigned int bits, unsigned int size )
{
  unsigned int sym_idx;

  /* Is this a leaf node? */
  if( node->Symbol >= 0 )
  {
    /* Append symbol to tree description */
    // _Huffman_WriteBits( stream, 1, 1 );
    // _Huffman_WriteBits( stream, node->Symbol, 8 );

    /* Find symbol index */
    for( sym_idx = 0; sym_idx < size; ++ sym_idx )
    {
      if( sym[sym_idx].Symbol == node->Symbol ) break;
    }

    /* Store code info in symbol array */
    sym[sym_idx].Code = code;
    sym[sym_idx].Bits = bits;
    // std::cout << "Symbol: "<< sym[sym_idx].Symbol << " code: " << code << " bits: " << bits << "\n";
    return;
  }
  else
  {
    /* This was not a leaf node */
    // _Huffman_WriteBits( stream, 0, 1 );
  }

  /* Branch A */
  Huffman_Canonical_StoreTree( node->ChildA, sym, stream, (code<<1)+0, bits+1, size);

  /* Branch B */
  Huffman_Canonical_StoreTree( node->ChildB, sym, stream, (code<<1)+1, bits+1, size);
}

/*************************************************************************
* _Huffman_StoreTree() - Store a Huffman tree in the output stream and
* in a look-up-table (a symbol array).
*************************************************************************/

static void _Huffman_StoreTree( huff_encodenode_t *node, huff_sym_t *sym,
  huff_bitstream_t *stream, unsigned int code, unsigned int bits )
{
  unsigned int sym_idx;

  /* Is this a leaf node? */
  if( node->Symbol >= 0 )
  {
    /* Append symbol to tree description */
    _Huffman_WriteBits( stream, 1, 1 );
    _Huffman_WriteBits( stream, node->Symbol, 8 );

    /* Find symbol index */
    for( sym_idx = 0; sym_idx < 256; ++ sym_idx )
    {
      if( sym[sym_idx].Symbol == node->Symbol ) break;
    }

    /* Store code info in symbol array */
    sym[sym_idx].Code = code;
    sym[sym_idx].Bits = bits;
    // std::cout << "code: " << code << " bits: " << bits << "\n";
    return;
  }
  else
  {
    /* This was not a leaf node */
    _Huffman_WriteBits( stream, 0, 1 );
  }

  /* Branch A */
  _Huffman_StoreTree( node->ChildA, sym, stream, (code<<1)+0, bits+1 );

  /* Branch B */
  _Huffman_StoreTree( node->ChildB, sym, stream, (code<<1)+1, bits+1 );
}


/*************************************************************************
* _Huffman_MakeTree() - Generate a Huffman tree.
*************************************************************************/

static void _Huffman_MakeTree( huff_sym_t *sym, huff_bitstream_t *stream )
{
  huff_encodenode_t nodes[MAX_TREE_NODES], *node_1, *node_2, *root;
  unsigned int k, num_symbols, nodes_left, next_idx;

  /* Initialize all leaf nodes */
  num_symbols = 0;
  for( k = 0; k < 256; ++ k )
  {
    if( sym[k].Count > 0 )
    {
      nodes[num_symbols].Symbol = sym[k].Symbol;
      nodes[num_symbols].Count = sym[k].Count;
      nodes[num_symbols].ChildA = (huff_encodenode_t *) 0;
      nodes[num_symbols].ChildB = (huff_encodenode_t *) 0;
      ++ num_symbols;
    }
  }

  /* Build tree by joining the lightest nodes until there is only
     one node left (the root node). */
  root = (huff_encodenode_t *) 0;
  nodes_left = num_symbols;
  next_idx = num_symbols;
  while( nodes_left > 1 )
  {
    /* Find the two lightest nodes */
    node_1 = (huff_encodenode_t *) 0;
    node_2 = (huff_encodenode_t *) 0;
    for( k = 0; k < next_idx; ++ k )
    {
      if( nodes[k].Count > 0 )
      {
        if( !node_1 || (nodes[k].Count <= node_1->Count) )
        {
          node_2 = node_1;
          node_1 = &nodes[k];
        }
        else if( !node_2 || (nodes[k].Count <= node_2->Count) )
        {
          node_2 = &nodes[k];
        }
      }
    }

    /* Join the two nodes into a new parent node */
    root = &nodes[next_idx];
    root->ChildA = node_1;
    root->ChildB = node_2;
    root->Count = node_1->Count + node_2->Count;
    root->Symbol = -1;
    node_1->Count = 0;
    node_2->Count = 0;
    ++ next_idx;
    -- nodes_left;
  }

  /* Store the tree in the output stream, and in the sym[] array (the
      latter is used as a look-up-table for faster encoding) */
  if( root )
  {
    _Huffman_StoreTree( root, sym, stream, 0, 0 );
  }
  else
  {
    /* Special case: only one symbol => no binary tree */
    root = &nodes[0];
    _Huffman_StoreTree( root, sym, stream, 0, 1 );
  }
}


/*************************************************************************
* _Huffman_RecoverTree() - Recover a Huffman tree from a bitstream.
*************************************************************************/

static huff_decodenode_t * _Huffman_RecoverTree( huff_decodenode_t *nodes,
  huff_bitstream_t *stream, unsigned int *nodenum )
{
  huff_decodenode_t * this_node;

  /* Pick a node from the node array */
  this_node = &nodes[*nodenum];
  *nodenum = *nodenum + 1;

  /* Clear the node */
  this_node->Symbol = -1;
  this_node->ChildA = (huff_decodenode_t *) 0;
  this_node->ChildB = (huff_decodenode_t *) 0;

  /* Is this a leaf node? */
  if( _Huffman_ReadBit( stream ) )
  {
    /* Get symbol from tree description and store in lead node */
    this_node->Symbol = _Huffman_Read8Bits( stream );

    return this_node;
  }

  /* Get branch A */
  this_node->ChildA = _Huffman_RecoverTree( nodes, stream, nodenum );

  /* Get branch B */
  this_node->ChildB = _Huffman_RecoverTree( nodes, stream, nodenum );

  return this_node;
}



/*************************************************************************
*                            PUBLIC FUNCTIONS                            *
*************************************************************************/


/*************************************************************************
* Huffman_Compress() - Compress a block of data using a Huffman coder.
*  in     - Input (uncompressed) buffer.
*  out    - Output (compressed) buffer. This buffer must be 384 bytes
*           larger than the input buffer.
*  insize - Number of input bytes.
* The function returns the size of the compressed data.
*************************************************************************/

int Huffman_Compress( unsigned char *in, unsigned char *out,
  unsigned int insize )
{
  huff_sym_t       sym[256], tmp;
  huff_bitstream_t stream;
  unsigned int     k, total_bytes, swaps, symbol;

  /* Do we have anything to compress? */
  if( insize < 1 ) return 0;

  /* Initialize bitstream */
  _Huffman_InitBitstream( &stream, out );

  /* Calculate and sort histogram for input data */
  _Huffman_Hist( in, sym, insize );

  /* Build Huffman tree */
  _Huffman_MakeTree( sym, &stream );

  /* Sort histogram - first symbol first (bubble sort) */
  do
  {
    swaps = 0;
    for( k = 0; k < 255; ++ k )
    {
      if( sym[k].Symbol > sym[k+1].Symbol )
      {
        tmp      = sym[k];
        sym[k]   = sym[k+1];
        sym[k+1] = tmp;
        swaps    = 1;
      }
    }
  }
  while( swaps );

  /* Encode input stream */
  for( k = 0; k < insize; ++ k )
  {
    symbol = in[k];
    _Huffman_WriteBits( &stream, sym[symbol].Code,
                        sym[symbol].Bits );
  }

  /* Calculate size of output data */
  total_bytes = (int)(stream.BytePtr - out);
  if( stream.BitPos > 0 )
  {
    ++ total_bytes;
  }

  return total_bytes;
}


/*************************************************************************
* Huffman_Uncompress() - Uncompress a block of data using a Huffman
* decoder.
*  in      - Input (compressed) buffer.
*  out     - Output (uncompressed) buffer. This buffer must be large
*            enough to hold the uncompressed data.
*  insize  - Number of input bytes.
*  outsize - Number of output bytes.
*************************************************************************/

void Huffman_Uncompress( unsigned char *in, unsigned char *out,
  unsigned int insize, unsigned int outsize )
{
  huff_decodenode_t nodes[MAX_TREE_NODES], *root, *node;
  huff_bitstream_t  stream;
  unsigned int      k, node_count;
  unsigned char     *buf;

  /* Do we have anything to decompress? */
  if( insize < 1 ) return;

  /* Initialize bitstream */
  _Huffman_InitBitstream( &stream, in );

  /* Recover Huffman tree */
  node_count = 0;
  root = _Huffman_RecoverTree( nodes, &stream, &node_count );

  /* Decode input stream */
  buf = out;
  for( k = 0; k < outsize; ++ k )
  {
    /* Traverse tree until we find a matching leaf node */
    node = root;
    while( node->Symbol < 0 )
    {
      /* Get next node */
      if( _Huffman_ReadBit( &stream ) )
        node = node->ChildB;
      else
        node = node->ChildA;
    }

    /* We found the matching leaf node and have the symbol */
    *buf ++ = (unsigned char) node->Symbol;
  }
}

/*************************************************************************
* Huffman_Canonical() - Generate Canonical Huffman Codes
*  in     - Input (uncompressed) buffer.
*  insize - Number of input bytes.
*************************************************************************/

void Huffman_Canonical(int input[], unsigned int insize, int output[]){
  huff_bitstream_t* stream;
  unsigned int     k, swaps, symbol;
  huff_sym_t       tmp;

  huff_sym_t* sym = new huff_sym_t[insize];
  /* Clear/init histogram */
  for( k = 0; k < insize; k++ )
  {
    sym[k].Symbol = k;
    sym[k].Count  = input[k];
    sym[k].Code   = 0;
    sym[k].Bits   = 0;
  }

  /* Build histogram */
  // for( k = 0; k < insize; k++ )
  // { 
  //   std::cout << k << "th input: " << input[k] << "\n";
  //   sym[input[k]].Count ++;
  // }

  /*Initialize all leaf nodes*/
  huff_encodenode_t nodes[MAX_TREE_NODES], *node_1, *node_2, *root;
  unsigned int num_symbols, nodes_left, next_idx;
  num_symbols = 0;
  for( k = 0; k < insize; k++ )
  {
    if( sym[k].Count > 0 )
    {
      // std::cout << sym[k].Symbol << " " << sym[k].Count << " " << sym[k].Code << " " << sym[k].Bits << "\n";
      nodes[num_symbols].Symbol = sym[k].Symbol;
      nodes[num_symbols].Count = sym[k].Count;
      nodes[num_symbols].ChildA = (huff_encodenode_t *) 0;
      nodes[num_symbols].ChildB = (huff_encodenode_t *) 0;
      ++ num_symbols;
    }
  }
  // std::cout << "num_symbols: " << num_symbols << "\n";
  /* Build tree by joining the lightest nodes until there is only
     one node left (the root node). */
  root = (huff_encodenode_t *) 0;
  nodes_left = num_symbols;
  next_idx = num_symbols;
  while( nodes_left > 1 )
  {
    /* Find the two lightest nodes */
    node_1 = (huff_encodenode_t *) 0;
    node_2 = (huff_encodenode_t *) 0;
    for( k = 0; k < next_idx; ++ k )
    {
      if( nodes[k].Count > 0 )
      {
        if( !node_1 || (nodes[k].Count <= node_1->Count) )
        {
          node_2 = node_1;
          node_1 = &nodes[k];
        }
        else if( !node_2 || (nodes[k].Count <= node_2->Count) )
        {
          node_2 = &nodes[k];
        }
      }
    }

    /* Join the two nodes into a new parent node */
    root = &nodes[next_idx];
    root->ChildA = node_1;
    root->ChildB = node_2;
    root->Count = node_1->Count + node_2->Count;
    root->Symbol = -1;
    node_1->Count = 0;
    node_2->Count = 0;
    ++ next_idx;
    -- nodes_left;
  }
  /* Store the tree in the sym[] array (the
      latter is used as a look-up-table for faster encoding) */
  if( root )
  {
    Huffman_Canonical_StoreTree( root, sym, stream, 0, 0, insize);
  }
  else
  {
    /* Special case: only one symbol => no binary tree */
    root = &nodes[0];
    Huffman_Canonical_StoreTree( root, sym, stream, 0, 1, insize);
  }

  /* Sort histogram - first symbol first (bubble sort) */
  do
  {
    swaps = 0;
    for( k = 0; k < insize - 1; ++ k )
    {
      if( sym[k].Symbol > sym[k+1].Symbol )
      {
        tmp      = sym[k];
        sym[k]   = sym[k+1];
        sym[k+1] = tmp;
        swaps    = 1;
      }
    }
  }
  while( swaps );

  for(uint i = 0; i < insize; i++){
    output[i] = sym[i].Bits;
    // std::cout << sym[i].Symbol << " " << sym[i].Count << " " << sym[i].Code << " " << sym[i].Bits << "\n";
  }

  /*create table, length: number of elements(bit_ranges), calculate header*/

  /*create tbale, bit_ranges, length, calculate actual size*/

  /*get canonical code for each symbol*/
  //todo
}