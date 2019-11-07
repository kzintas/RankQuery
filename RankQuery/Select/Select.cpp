#include"Select.h"


select_support::select_support(rank_support* R) {
		this->r1 = R;
}

uint64_t select_support::select1(uint64_t i) {
		// : Returns the position, in the underlying bit - vector, of the ith 1.

		int left = 1;
		int right = r1->size;
		int rank_val = 0;
		int middle;//middle
		bool flag = false;
		while (left <= right) {
			middle = (left + right) / 2;
			rank_val = r1->rank1(middle);
			if (rank_val > i) right = middle - 1;
			else if (rank_val < i) left = middle + 1;
			else {
				flag = true;
				break;
			}
		}

		if (flag == false) return 0;
		while (r1->b[middle - 1] == 0) {
			middle -= 1;
		}
		return middle;
	}


uint64_t select_support::select0(uint64_t i) {
		// : Returns the the position, in the underlying bit - vector, of the ith 0.

		int left = 1;
		int right = r1->size;
		int rank_val = 0;
		int middle;//middle
		bool flag = false;
		while (left <= right) {
			middle = (left + right) / 2;
			rank_val = r1->rank0(middle);
			if (rank_val > i) right = middle - 1;
			else if (rank_val < i) left = middle + 1;
			else {
				flag = true;
				break;
			}
		}

		if (flag == false) return 0;
		while (r1->b[middle - 1] == 1) {
			middle -= 1;
		}
		return middle;
	}

    uint64_t select_support:: overhead() {
		// : Returns the size of the select data structure(in bits) that is required to support log - time select on the current bitvector(how does this relate to the size of the rank data structure built above).
		r1->overhead();

	
	}