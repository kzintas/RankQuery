#include "Rank.h"

superblock::superblock(int a) {
	value = a;
}

block::block(int a ) {
	value = a;
		
}

block::block(superblock& S, int a ) {
	value = a;
		S_b = S;
}


rank_support::rank_support(bit_vector& b) {
	this->b = b;

	size = b.size();//bit_vector_size

	//Calculation for superblock

	if (size > 1) {
		superblock_size = ceil(pow(log2(b.size()), 2.0) / 2.0);	// Number of elements to be summed per superblock

		number_of_superblocks = ceil((double)b.size() / superblock_size);		//Number of superblocks
			
			//Calculation for Blocks inside each superblock
		block_size = ceil(log2(b.size()) / 2.0); // Number of elements to be summed up per block
		number_of_blocks = ceil((double)superblock_size / block_size); //Number of blocks per superblock
	}
	else {
		superblock_size = 1;
		block_size = 1;
	}
		
	int count = 0;	//Redundant var, keeping track
	R_s.push_back(0);	//First value will always be zero

	//Superblock calculations
	int sum = 0;
			
	for (int i = 0; i < (number_of_superblocks - 1); i++) {	//Loop over number of super blocks
		for (int j = 0; (j < superblock_size && count < size); j++, count++) {	//Loop over the elements
			sum += b[superblock_size * i + j];
	
		}
		
		R_s.push_back(sum);
	}

		//Block calculations
	count = 0;
	int scount = 0;
		
	for (int k = 0; k < R_s.size(); k++) {
		int sum2 = 0;
		scount = 0;
		for (int l = 0; l < number_of_blocks; l++) {
			block temp_b(R_s.at(k), sum2);
			R_b.push_back(temp_b);

			for (int n = 0; n < block_size && count < size && scount < superblock_size; n++, count++) {
		
				sum2 += b[k * superblock_size + l * block_size + n];
				scount++;
			}
				
		}
	}

	
	
	//In block Calculations
	int combinations = pow(2.0, block_size); //Number of possible calculations
	R_p = new int* [combinations];

	//cout << combinations<<"A\n";

	for (int m = 0; m < combinations; m++) {
		R_p[m] = new int[block_size];
	}

		//int checker = 1;

	bool v = false;
	for (int m = 0; m < combinations; m++) {
		v = m & (1 << (block_size - 1));
		R_p[m][0] = v;

			
		for (int n = 1; n < block_size; n++) {
			v = m & (1 << (block_size - n - 1));

			R_p[m][n] = R_p[m][n - 1] + v;
				
				
		}
			
			
	}

		
}

uint64_t rank_support:: rank0(uint64_t i) {
	//: Returns the number of 0s in the underlying bit - vector up to position i(inclusive) � simply i - rank1(i).
	if(i==0) return 0;
	return i - rank1(i);
}
	
uint64_t rank_support:: overhead() {
		//: Returns the size of the rank data structure(in bits) that is required to support constant - time rank on the current bitvector.

	int size = sizeof(b) * b.size() + sizeof(R_s) * R_s.size() * 8 + sizeof(R_b) * R_b.size() * 8 + sizeof(R_p) * 8 + pow(2.0, block_size) * block_size * sizeof(int) * 8;
	//cout << "\n" << size << "\n";
	//cout << sizeof(b) << "\t" << sizeof(R_b) << "\t" << sizeof(R_s);
	return size;
}
	
	