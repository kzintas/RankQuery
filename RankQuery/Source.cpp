#include <vector>
#include <string>
#include<iostream>
#include<cmath>
#include<bitset>
#include <algorithm>
#include <numeric>
#include <set>

using namespace std;

typedef std::vector<bool> bit_vector;

//typedef std::bitset<200> bit_vector;

class block;
class superblock;

class superblock {
public:
	int value;
	superblock(int a = 0) {
		value = a;
	}

	void set_val(int a) {
		value = a;
	}
	int get_val() {
		return value;
	}
};

class block {
public:
	int value;
	superblock S_b;
	block(superblock& S, int a = 0) {
		//this->S = S;
		value = a;
		S_b = S;
	}

	block(int a = 0) {
		//this->S = S;
		value = a;
		//S_b = S;
	}
	void set_val(int a) {
		value = a;
	}
	int get_val() {
		return value;
	}
};

class rank_support {
public:
	bit_vector b;
	vector<superblock> R_s;
	vector<block> R_b;
	int** R_p;
	int boundary_sb;
	int boundary_b;
	int number_b;
	int size;

	rank_support(bit_vector& b) {
		this->b = b;

		size = b.size();//bit_vector_size

		//Calculation for superblock

		boundary_sb = ceil(pow(log2(b.size()), 2.0) / 2.0);	// Number of elements to be summed per superblock
		int number_sb = ceil((double)b.size() / boundary_sb);		//Number of superblocks
		//int count_sb = size_sb / boundary_sb;

		//cout <<b.size()<<"\t"<< size_sb << "\t"<< boundary_sb <<"\n";

		//Calculation for Blocks inside each superblock
		boundary_b = ceil(log2(b.size()) / 2.0); // Number of elements to be summed up per block
		number_b = ceil((double)boundary_sb / boundary_b); //Number of blocks per superblock

		//cout << b.size() << "\t" << boundary_sb << "\t" << number_sb << "\t"<<boundary_b << "\t" << number_b<<  "\n";

		int count = 0;	//Redundant var, keeping track
		R_s.push_back(0);	//First value will always be zero

		//Superblock calculations
		for (int i = 0; i < (number_sb - 1) ; i++) {	//Loop over number of super blocks
			int sum = 0;
			for (int j = 0; (j < boundary_sb && count < size); j++, count++) {	//Loop over the elements
				sum += b[boundary_sb * i + j];
				//cout << j << "\n";
				
			}
			//cout << sum<< "\t "<< "valuestored"<<"\n";
			R_s.push_back(sum);
		}

		//Block calculations
		count = 0;
		
		//R_b.push_back(block(R_s.at(0), 0));

		for (int k = 0; k < R_s.size(); k++) {
			int sum2 = 0;
			for (int l = 0; l < (boundary_sb - boundary_b + 1) ; l += boundary_b) {
				block temp_b(R_s.at(k), sum2);
				R_b.push_back(temp_b);
				
				for (int n = 0; n < boundary_b && count < size; n++ , count++) {
					//cout << k * boundary_sb + l + n << "\t"<<l<<"\t"<<n<<"\n";
					sum2 += b[k * boundary_sb + l + n];
				}
				//cout << sum2;
			}
		}

		//In block Calculations
		int combinations = pow(2.0, boundary_b); //Number of possible calculations
		R_p = new int* [combinations];

		//cout << combinations<<"A\n";

		for (int m = 0; m < combinations; m++) {
			R_p[m] = new int[boundary_b];
		}

		//int checker = 1;

		bool v = false;
		for (int m = 0; m < combinations; m++) {
			v = m & (1 << (boundary_b - 1));
			R_p[m][0] = v;

			//cout << R_p[m][0] << "\t";
			for (int n = 1; n < boundary_b; n++) {
				v = m & (1 << (boundary_b - n - 1));

				R_p[m][n] = R_p[m][n - 1] + v;
				//cout << R_p[m][n]<<"\t";
			}
			//cout << "\n";
		}

		//cout << b.size()<< "\t"<<log2(b.size())<<"\t"<<ceil(log2(b.size()));
	}

	uint64_t rank1(uint64_t i) {
		i -= 1;
		int superblock_index = floor(i / boundary_sb);
		int block_index = floor(i / boundary_b);

		int block_start = block_index * boundary_b;
		int index_entry_in_block = i % block_start;

		int block_end = min(block_start + boundary_b, boundary_sb + superblock_index - 1);

		//Option 2- Accumulator

		vector<bool>::const_iterator first = b.begin() + block_start;
		vector<bool>::const_iterator last = b.begin() + block_start + boundary_b;

		vector<bool> newVec(first, last);
		//oo = std::vector(b.begin () + 5, b.begin () + 7);
		//auto p = newVec.begin()._Myptr;
		//cout << *p << endl;

		i = accumulate(newVec.begin(), newVec.end(), 0, [](int x, int y) { return (x << 1) + y; });

		//OPTION-1 Use STRING
		/*
		std::string s = "";
		for (int j = block_start; j < block_end; j++) {
			s += std::to_string(b[j]);
		}

		int result = std::stoi(s,0,2);
		*/
		// cout << this->R_s[superblock_index].value << "\t" << this->R_b[block_index].value << "\t" << this->R_p[result][index_entry_in_block] << "n";

		return this->R_s[superblock_index].value + this->R_b[block_index].value + this->R_p[i][index_entry_in_block];
	}
	//: Returns the number of 1s in the underlying bit - vector up to position i(inclusive).

	uint64_t rank0(uint64_t i) {
		return i - rank1(i);
	}
	//: Returns the number of 0s in the underlying bit - vector up to position i(inclusive) — simply i - rank1(i).

	uint64_t overhead() {
		cout << sizeof(b) << "\t" << sizeof(R_b) << "\t" << sizeof(R_s);
		return sizeof(b);
	}
	//: Returns the size of the rank data structure(in bits) that is required to support constant - time rank on the current bitvector.

	/*
	uint64_t rank1_for_select(uint64_t i) {
		i -= 1;
		int superblock_index = floor(i / boundary_sb);
		int block_index = floor(i / boundary_b);

		return this->R_s[superblock_index].value + this->R_b[block_index].value ;
	}
	*/
};

class select_support {
public:
	rank_support* r1;
	select_support(rank_support* R) {
		this->r1 = R;
	}
	uint64_t select1(uint64_t i) {
		int left = 1;
		int right = r1->size;
		int rank_val = 0;
		int middle;//middle
		bool flag = false;
		while (left < right) {
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
	}// : Returns the position, in the underlying bit - vector, of the ith 1.

	uint64_t select0(uint64_t i) {
		int left = 1;
		int right = r1->size;
		int rank_val = 0;
		int middle;//middle
		bool flag = false;
		while (left < right) {
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
	}// : Returns the the position, in the underlying bit - vector, of the ith 0.

	uint64_t overhead() {
	}// : Returns the size of the select data structure(in bits) that is required to support log - time select on the current bitvector(how does this relate to the size of the rank data structure built above).
};

class wavelet_tree {
public:

	std::set<char> alphabet;
	char middle;
	wavelet_tree* Right;
	wavelet_tree* Left;
	wavelet_tree* Parent;
	bit_vector B;
	rank_support *r1;
	wavelet_tree(string str, wavelet_tree* p) {
		Parent = p;

		string temp_left = "";
		string temp_right = "";
		alphabet.insert(str.begin(), str.end());
		

		//cout << alphabet.size();
		if (alphabet.size() == 1) return;
		middle = ((int) *alphabet.begin() + *alphabet.rbegin()) / 2.0;
		//cout << middle;
		for (auto a : str) {
			if (a <= middle) {
				B.push_back(0);
				temp_left += a;
			}
			else {
				B.push_back(1);
				temp_right += a;
			}
		}
		r1 = new rank_support(B);
		if (!temp_left.empty()) {
			cout << temp_left << "\ttemp_left\n";
			Left = new wavelet_tree(temp_left, this);
		}
		if (!temp_right.empty()) {
			cout << temp_right << "\ttemp right\n";
			Right = new wavelet_tree(temp_right, this);
		}
	}
};

int main(void) {
	//std::string bits("10101011101011111000");

	wavelet_tree wt("helloworldo", NULL);
	/*
	std::string bits("1001011100001010");

	//bit_vector b(bits);

	bit_vector b;

	for (auto a : bits)
		b.push_back(a == '1');

	rank_support r1(b);
	rank_support* r = &r1;

	select_support s1(r);

	*/
	//cout << "\n" << s1.select0(7);
	//cout << "\n" << r1.rank1(13);

	//r1.overhead();
	/*
	std::vector<bool > bit_vector;

	bit_vector.push_back(false);
	std::vector<bool >::reference ref1 = bit_vector.at(0);
	cout << ref1 << endl; // ref1 implicitly cast to bool

	bool b1;

	// one form of an explicit cast
	b1 = ref1.operator bool();
	cout << b1 << endl;

	// another form of an explicit cast
	b1 = bool(ref1);
	cout << b1 << endl;
	*/
	return 0;
}