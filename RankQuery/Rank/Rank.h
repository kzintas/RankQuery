//#define _GLIBCXX_USE_CXX11_ABI 0
#include <vector>
#include<cmath>
#include <algorithm>
#include <numeric>
#include <stdint.h>

typedef std::vector<bool> bit_vector;
//using namespace std;

class block;
class superblock;

class superblock {
public:
	int value;
	superblock(int a = 0);

};

class block {
public:
	int value;
	superblock S_b;
	block(superblock& S, int a = 0);

	block(int a = 0);
};

class rank_support {
public:
	bit_vector b;
	std::vector<superblock> R_s;
	std::vector<block> R_b;
	int** R_p;
	int superblock_size;
	int block_size;
	int number_of_blocks;
	int number_of_superblocks;
	int size;

	rank_support(bit_vector& b);

	uint64_t rank1(uint64_t i) {
		//: Returns the number of 1s in the underlying bit - vector up to position i(inclusive).
		if(i<=0) return 0;
		
		i -= 1;
		//if(i==0) return 0;
		int superblock_index = floor(i / superblock_size);
		int block_index = floor((i - superblock_index * superblock_size) / block_size);

		int block_start = (superblock_index * superblock_size) + block_index * block_size;
		int index_entry_in_block = i;
		if (block_start != 0) index_entry_in_block = i % block_start;

		int block_end = std::min(std::min(block_start + block_size, superblock_size * (superblock_index + 1)), size);

		//Option 2- Accumulator

		bit_vector::const_iterator first = b.begin() + block_start;
		bit_vector::const_iterator last = b.begin() + block_end;

		bit_vector newVec(first, last);
		
		
		while(block_end<(block_start+block_size)){
			newVec.push_back(false);
			block_end++;
		}
		i = accumulate(newVec.begin(), newVec.end(), 0, [](int x, int y) { return (x << 1) + y; });

		//OPTION-1 Use STRING
		/*
		std::string s = "";
		for (int j = block_start; j < block_end; j++) {
			s += std::to_string(b[j]);
		}

		int result = std::stoi(s,0,2);
		*/
		
		
		return this->R_s[superblock_index].value + this->R_b[number_of_blocks * superblock_index + block_index].value + this->R_p[i][index_entry_in_block];
	}
	
	uint64_t rank0(uint64_t i);
	
	uint64_t overhead() ;
	
	
	
};