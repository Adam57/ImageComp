# compiler flags
CC=g++
CFLAGS=-c -Wall -std=c++11

all: analysis

analysis: test.o entropy.o reorder.o rebuilder.o var_bits.o bits_utils.o huffman.o move_to_front.o compressor.o
	$(CC) test.o entropy.o reorder.o rebuilder.o var_bits.o bits_utils.o huffman.o move_to_front.o compressor.o -o analysis

compressor.o: compressor.cc compressor.h
	$(CC) $(CFLAGS) compressor.cc

test.o: test.cc data_analysis.h entropy.h
	$(CC) $(CFLAGS) test.cc

# data_analysis.o: data_analysis.cc data_analysis.h
# 	$(CC) $(CFLAGS) data_analysis.cc

entropy.o: entropy.cc entropy.h
	$(CC) $(CFLAGS) entropy.cc

# delta_encoding.o: delta_encoding.cc delta_encoding.h entry.h
# 	$(CC) $(CFLAGS) delta_encoding.cc

huffman.o: huffman.cc huffman.h
	$(CC) $(CFLAGS) huffman.cc

move_to_front.o: move_to_front.cc move_to_front.h
	$(CC) $(CFLAGS) move_to_front.cc

# variable_byte_encoding.o: variable_byte_encoding.cc variable_byte_encoding.h
# 	$(CC) $(CFLAGS) variable_byte_encoding.cc

# rerank_keypoints.o: ../include/rerank_keypoints.h ../source/rerank_keypoints.cpp
# 	$(CC) $(CFLAGS) ../source/rerank_keypoints.cpp

reorder.o: reorder.cc reorder.h entry.h
	$(CC) $(CFLAGS) reorder.cc

rebuilder.o: rebuilder.cc rebuilder.h entry.h
	$(CC) $(CFLAGS) rebuilder.cc

var_bits.o: var_bits.cc var_bits.h
	$(CC) $(CFLAGS) var_bits.cc

bits_utils.o: bits_utils.cc bits_utils.h
	$(CC) $(CFLAGS) bits_utils.cc

clean:
	rm -rf *o analysis