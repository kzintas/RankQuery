#include <vector>
#include <string>
#include<iostream>
#include<cmath>
#include<bitset>


using namespace std;

typedef std::vector<bool> bit_vector;

//typedef std::bitset<200> bit_vector;


class block;
class superblock; 


class superblock {

	
	int value;
	
public:
	superblock(int a=0) {
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


	int value;
	superblock S_b;
public:
	block(superblock &S,int a = 0) {
		//this->S = S;
		value = a;
		S_b = S;

	}

	block( int a = 0) {
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

	 bit_vector b;
	 vector<superblock> R_s;
	 vector<block> R_b;
	 int** R_p;
	 

 public:
	 rank_support(bit_vector &b) {
		 this->b = b;
		 
		 int size = b.size();//block_size

		 //Calculation for superblock
		 
		 int boundary_sb = ceil(pow(log2(b.size()), 2.0) / 2.0);	// Number of elements to be summed per superblock
		 int number_sb = ceil ((double) b.size() / boundary_sb);		//Number of superblocks
		 //int count_sb = size_sb / boundary_sb;

		 //cout <<b.size()<<"\t"<< size_sb << "\t"<< boundary_sb <<"\n";
		 
		 //Calculation for Blocks inside each superblock
		 int boundary_b = ceil(log2(b.size()) / 2.0); // Number of elements to be summed up per block
		 int number_b = ceil ( (double) boundary_sb / boundary_b); //Number of blocks per superblock

		 //cout << b.size() << "\t" << boundary_sb << "\t" << number_sb << "\t"<<boundary_b << "\t" << number_b<<  "\n";
		 

		 //int count = 0;	//Redundant var, keeping track
		 R_s.push_back(0);	//First value will always be zero


		 //Superblock calculations
		 for (int i = 0; i <(number_sb -1) ; i++) {	//Loop over number of super blocks
			 int sum = 0;
			 for (int j = 0; j < boundary_sb; j++) {	//Loop over the elements
				 sum +=  b[boundary_sb *i+j];
				 //cout << j << "\n";
				 //count++;
			 }
			 //cout << sum<< "\t "<< "valuestored"<<"\n";
			 R_s.push_back(sum);

		 }

		 
		 //Block calculations
		 for (int k = 0; k < R_s.size(); k++) {
			 int sum2 = 0;
			 for (int l = 0; l < (boundary_sb-boundary_b+1); l+=boundary_b) {
				 block temp_b(R_s.at(k), sum2);
				 R_b.push_back(temp_b);

				 for (int n = 0; n < boundary_b ; n++) {
					 //cout << k * boundary_sb + l + n << "\t"<<l<<"\t"<<n<<"\n";
					 sum2 += b[k*boundary_sb+l+n];

				 }
				 //cout << sum2;
				 
			 }
				 

		 }

		 //In block Calculations
		 int combinations = pow(2.0, boundary_b); //Number of possible calculations
		 R_p = new int* [combinations];
		 
		 cout << combinations<<"A\n";

		 for (int m = 0; m < combinations; m++) {
			 R_p[m] = new int[boundary_b];
		 }

		 int checker = 1;
		 
		 bool v = false;
		 for (int m = 0; m < combinations; m++) {
			  v= m & (1 << (boundary_b - 1));
			  R_p[m][0] = v;

			  cout << R_p[m][0] << "\t";
			 for (int n = 1; n < boundary_b; n++) {
				 v = m & (1 << (boundary_b - n - 1));

				 R_p[m][n] = R_p[m][n - 1] + v;
				 cout << R_p[m][n]<<"\t";
			 }
			 cout << "\n";
			 
		 }

		 cout << b.size()<< "\t"<<log2(b.size())<<"\t"<<ceil(log2(b.size()));

		 
		 
	}
	 


};

int main(void) {

	//std::string bits("10101011101011111000");
	std::string bits("1001011101001010");
	

	//bit_vector b(bits);
	
	bit_vector b;

	for (auto a : bits)
		b.push_back(a == '1');
	
	rank_support r1(b);
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

