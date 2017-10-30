/*************************************************************************
* Name:        move_to_front.cc
* Author:      Qi Wang
* Description: MTF coder/decoder interface.
* Copyright:   2017 Blippar
*-------------------------------------------------------------------------
*************************************************************************/
#include "move_to_front.h"
#define CACHESIZE 8
#include <iostream>
#include <cstdlib>

void getDiffValues(std::vector<int> &input, std::vector<int> &output){

}

void remapAngle(std::vector<int> &angle_values){
	
}

int approxCopyCounter(std::vector<int> &input){
	int count = 0;
	for(uint i = 0; i < input.size(); i ++){
		if(input[i]%2 == 1){
			count ++;
		}
	}	
	return count;
}

int exactCopyCounter(std::vector<int> &input){
	int count = 0;
	int reminder = 0;
	for(uint i = 0; i < input.size(); i ++){
		if(input[i]%2 == 1){
			reminder = input[i];
			reminder /= 2;
			reminder /= CACHESIZE;
			reminder /= 2;
			if(reminder == 0){
				count ++;
			}
		}
	}	
	return count;
}

int copyTransform(int index){
	return index;
}

int literalTransform(int number, int offset){
	return number + offset;
}

/*use bit stream*/
// int copyTransformApprox(int diff, int index, int sign){
// 	return CACHESIZE * (2 * diff + sign) + index;
// }

// int literalTransformApprox(int number){
// 	return number;
// }

int copyTransformApprox(int diff, int index, int sign){
	return 2 * ( CACHESIZE * (2 * diff + sign) + index) + 1;
}

int literalTransformApprox(int number){
	return 2 * number;
}

void moveElemInCache(int* cache, int cur_cache_size, int from, int to){
	// std::cout << "from: " << from << " to " << to << "\n";
	if( (from >= cur_cache_size) || (to > from) ){
		std::cout << "check index in function moveElemInCache\n";
		exit(0);
	}
	int tmp = cache[from];
	for(int i = from; i > to; i--){
		cache[i] = cache[i-1];
	}
	cache[to] = tmp;
}

int insertElemToCache(int* cache, int cur_cache_size, int value, int to){
	if( (to > cur_cache_size) || (to >= CACHESIZE) ){
		std::cout << "check index in function insertElemToCache\n";
		exit(0);
	}

	if(cur_cache_size < CACHESIZE){
		cur_cache_size ++;
	}
	if(cur_cache_size < CACHESIZE){
		for(int i = cur_cache_size; i > to; i--){
			cache[i] = cache[i - 1];
		}
	}else{
		for(int i = cur_cache_size - 1; i > to; i--){
			cache[i] = cache[i - 1];
		}
	}
	cache[to] = value;
	return cur_cache_size;
}

void displayComparison(std::vector<int> &input){
	int sign = 0;
	int offset = 0;
	int index = 0;
	int reminder = 0;
	std::vector<int> num;
	int original_num = 0;

	/*cache*/
	int cur_cache_size = 0;
	int cache[CACHESIZE];

	for(uint i = 0; i < input.size(); i ++){
		// std::cout <<"code: " << input[i];
		if(input[i]%2 == 1){
			reminder = input[i];
			reminder /= 2;
			index = reminder%CACHESIZE;
			reminder /= CACHESIZE;
			sign = reminder%2;
			reminder /= 2;
			offset = reminder;
			/*without move to front in the cache*/
			original_num = ((sign == 1)? 1:-1)*offset + cache[index];
			// std::cout << "sign: " << sign << " offset: " << offset << " index: " << index << " base number: " << cache[index] << " orignal number: " << original_num;	
			std::cout << original_num << " is a copy from cache position: " << index << " with sign: " << sign << " offset: " << offset << " resulting in code: " << input[i];
			// moveElemInCache(cache, cur_cache_size, min_offset, 0);
			moveElemInCache(cache, cur_cache_size, index, 0);
		}else{
			original_num = input[i]/2;
			std::cout << original_num << " is a literal resulting in code: " << input[i];
		}
		// num.push_back(original_num);
		cur_cache_size = insertElemToCache(cache, cur_cache_size, original_num, 0);
		std::cout << "\n";
	}	
	std::cout << "image length: " << input.size();
	std::cout << " --- end of document ---\n";
}

void moveToFrontApprox(std::vector<int> &input){
	/*number of items currently in cache*/
	int cur_cache_size = 0;
	/*cache*/
	int cache[CACHESIZE];
	int size = input.size();
	/*output*/
	std::vector<int> output;
	output.resize(size);
	// std::cout << size << "\n";
	// for(int i = 0; i < size; i++){
	// 	output.push_back(0);
	// }
	// std::cout << output.size() << "\n";
	/*sign and difference*/
	int sign = 0;
	int diff = 0;
	/*copy value*/
	int copy_value = 0;
	/*closest same number to current number*/
	int min_offset = 0; 
	int min_copy_value = 0;
	int literal = 0;

	for(int i = 0; i < size; i++){
		// std::cout << "input number: " << input[i] <<"\n";
		literal = literalTransformApprox(input[i]);
		min_copy_value = literal;
		// std::cout << "cache: \n";
		for(int j = 0; j < cur_cache_size; j++){
			// std::cout << cache[j] << " ";
			sign = (input[i] > cache[j])? 1:0;
			diff = std::abs(input[i] - cache[j]);
			copy_value = copyTransformApprox(diff, j, sign);
			if(copy_value < min_copy_value){
				min_offset = j;
				min_copy_value = copy_value;
			}
		}
		// std::cout << "\n";
		if(min_copy_value < literal){
			// std::cout << "move element in cache\n";
			output[i] = min_copy_value;
			/*move the element to the first position in cache*/
			moveElemInCache(cache, cur_cache_size, min_offset, 0);
		}else{
			// std::cout << "take literal " << "\n";
			// std::cout << "output before: " << output[i] << "\n";
			// std::cout << "literal: " << literal << "\n";
			output[i] = literal;
			// std::cout << "output after: " << output[i] << "\n";
		}
		// std::cout << "insert element in cache\n";
		cur_cache_size = insertElemToCache(cache, cur_cache_size, input[i], 0);
	}

	input = output;
}


void moveToFront(std::vector<int> &input){
	// std::cout << "move to front in action\n";
	/*number of items currently in cache*/
	int cur_cache_size = 0;
	/*closest same number to current number*/
	int min_offset = 0; 
	int min_offset_value = 0;
	bool cache_hit = false;
	/*cache*/
	int cache[CACHESIZE];
	int size = input.size();
	/*output*/
	std::vector<int> output;
	output.resize(size);
	// for(int i = 0; i < size; i++){
	// 	output.push_back(0);
	// }

	for(int i = 0; i < size; i++){
		// std::cout << "input number: " << input[i] <<"\n";
		min_offset_value = literalTransform(input[i], cur_cache_size);
		// std::cout << "cache: \n";
		for(int j = 0; j < cur_cache_size; j++){
			// std::cout << cache[j] << " ";
			if(input[i] == cache[j]){
				cache_hit = true;
				if(copyTransform(j) < min_offset_value){
					min_offset = j;
					min_offset_value = copyTransform(j);
				}
			}
		}
		// std::cout << "\n";
		/*if the number appears in cache before*/
		if(cache_hit){
			/*use offset value if offset value is smaller than actually value*/
			if( min_offset_value < literalTransform(input[i], cur_cache_size) ){
				output[i] = min_offset_value;
				// std::cout << "output: " << output[i] << "\n";
			}else{/*use actual value if offset value is larger than actually value*/
				output[i] = literalTransform(input[i], cur_cache_size);
				// std::cout << "output: " << output[i] << "\n";
			}

			/*move the element to the first position in cache*/
			moveElemInCache(cache, cur_cache_size, min_offset, 0);
		}else{ /*if the number does not appear in cache before*/
			// std::cout << input[i] << "\n";
			// std::cout << literalTransform(input[i], cur_cache_size) << "\n";
			// std::cout << "output is wrong for what\n";
			// std::cout << output[i] << "\n";
			output[i] = literalTransform(input[i], cur_cache_size);
			// std::cout << "output: " << output[i] << "\n";
			cur_cache_size = insertElemToCache(cache, cur_cache_size, input[i], 0);
		}

		// std::cout << "cur_cache_size: " << cur_cache_size <<"\n";
		// for(int i = 0; i < cur_cache_size; i++){
		// 	std::cout << cache[i] << " ";
		// }
		// std::cout << "\n";

		cache_hit = false;
	}

	input = output;
}