#include <vector>
#include <string>
#include<iostream>
#include<cmath>
#include<bitset>
#include <algorithm>
#include <numeric>
#include <set>
#include <fstream>
#include <chrono>
#include <ctime>

using namespace std;

typedef std::vector<bool> bit_vector;


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
		value = a;
		S_b = S;
	}

	block(int a = 0) {
		value = a;
		
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
	int superblock_size;
	int block_size;
	int number_of_blocks;
	int number_of_superblocks;
	int size;

	rank_support(bit_vector& b) {
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

	uint64_t rank1(uint64_t i) {
		//: Returns the number of 1s in the underlying bit - vector up to position i(inclusive).
		if(i<=0) return 0;
		

		i -= 1;
		int superblock_index = floor(i / superblock_size);
		int block_index = floor((i - superblock_index * superblock_size) / block_size);

		int block_start = (superblock_index * superblock_size) + block_index * block_size;
		int index_entry_in_block = i;
		if (block_start != 0) index_entry_in_block = i % block_start;

		int block_end = min(min(block_start + block_size, superblock_size * (superblock_index + 1)), size);

		//Option 2- Accumulator

		vector<bool>::const_iterator first = b.begin() + block_start;
		vector<bool>::const_iterator last = b.begin() + block_end;

		vector<bool> newVec(first, last);
		
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
	
	uint64_t rank0(uint64_t i) {
		//: Returns the number of 0s in the underlying bit - vector up to position i(inclusive) � simply i - rank1(i).
		if(i==0) return 0;
		return i - rank1(i);
	}
	
	uint64_t overhead() {
		//: Returns the size of the rank data structure(in bits) that is required to support constant - time rank on the current bitvector.

		int size = sizeof(b) * b.size() + sizeof(R_s) * R_s.size() * 8 + sizeof(R_b) * R_b.size() * 8 + sizeof(R_p) * 8 + pow(2.0, block_size) * block_size * sizeof(int) * 8;
		cout << "\n" << size << "\n";
		cout << sizeof(b) << "\t" << sizeof(R_b) << "\t" << sizeof(R_s);
		return size;
	}
	
	
	
};

class select_support {
public:
	rank_support* r1;
	select_support(rank_support* R) {
		this->r1 = R;
	}
	uint64_t select1(uint64_t i) {
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

	uint64_t select0(uint64_t i) {
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
	uint64_t overhead() {
		// : Returns the size of the select data structure(in bits) that is required to support log - time select on the current bitvector(how does this relate to the size of the rank data structure built above).
		r1->overhead();

	
	}
};

class wavelet_tree {
public:

	std::set<char> alphabet;
	char middle;
	bit_vector B;
	wavelet_tree* Right;
	wavelet_tree* Left;
	
	rank_support* r1;
	select_support* s1;

	wavelet_tree(string str) {
	
		string temp_left = "";
		string temp_right = "";
		alphabet.insert(str.begin(), str.end());

	
		middle = ((int)*alphabet.begin() + *alphabet.rbegin()) / 2.0;
		
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
		s1 = new select_support(r1);
		if (alphabet.size() == 1) return;
		if (!temp_left.empty()) {
			//cout << temp_left << "\ttemp_left\n";
			Left = new wavelet_tree(temp_left);
		}
		if (!temp_right.empty()) {
			//cout << temp_right << "\ttemp right\n";
			Right = new wavelet_tree(temp_right);
		}
	}

	wavelet_tree(string str, int offset) {
		
		if (str.substr(offset, 10) == "{alphabet:") {

			offset += 10;
			while (str.at(offset) != ';') {
				this->alphabet.insert(str.at(offset));
				//cout << str.at(offset);
				offset += 2;
			}
			offset++;
		}
		
		if (str.substr(offset , 5) == "bits:") {
			offset += 5;
			while (str.at(offset) != ';') {
				this->B.push_back(str.at(offset)=='1');
				//cout << str.at(offset);
				offset++;
			}
			offset++;
		}
		//cout << "\n";
		if (str.substr(offset, 7) == "middle:") {
			offset += 7;
			this->middle = str.at(offset);
			offset+=2;
		}
		r1 = new rank_support(B);
		s1 = new select_support(r1);

		if (str.substr(offset).find_first_not_of('}') == string::npos) return;
		if (str.substr(offset , 5) == "Left:") {
			offset += 6;
			this->Left = new wavelet_tree(str,offset);
			int count = 0;
			while (true) {
				if (str.at(offset) == '{') count++;
				if (str.at(offset) == '}') count--;
				offset++;
				if (count == 0)  break;
				
			}
		}
			
		
		
		if (str.at(offset) == '}') {
			offset++; 
			return; 
		}

		if (str.substr(offset , 6) == "Right:") {
			offset += 7;
			
			this->Right = new wavelet_tree(str, offset);
			int count = 0;
			while (true) {
				if (str.at(offset) == '{') count++;
				if (str.at(offset) == '}') count--;
				offset++;
				if (count == 0) break;
			}

		}
		return;
		
	}

	int rank(char c, int index) {
		while (this->Left != NULL && this->Right != NULL) {
			//cout << index << "\n";
			if (c <= middle) {
				index = this->r1->rank0(index);
				return this->Left->rank(c, index);
			}

			else {
				index = this->r1->rank1(index);
				return this->Right->rank(c, index);
			}
		}
		return index;
	}

	int select(char c, int occurrence) {
		int index;
		wavelet_tree* temp = this;
		while (alphabet.size() != 1) {
			//cout << occurrence << "\n";
			if (c <= middle) {
				occurrence = this->Left->select(c, occurrence);
				return this->s1->select0(occurrence);
			}

			else {
				occurrence = this->Right->select(c, occurrence);
				return this->s1->select1(occurrence);
			}
		}

		if (this->B.size() < occurrence) return 0;
		return occurrence;
	}


	char access(int index) {
		
		while (alphabet.size()!=1) {
			//cout << index <<"\t"<<alphabet.size() <<"\n";
			if (this->B.at(index)==0) {
				index = this->r1->rank0(index+1);
				return this->Left->access(index-1);
			}

			else {
				index = this->r1->rank1(index+1);
				return this->Right->access(index-1);
			}
		}
		//cout<<"Here"<<"\n";
		return (int)*alphabet.begin();
	}
	
};

void wavelet_tree_dump(std::ostream& fout, const wavelet_tree* node) {
	
	
	fout << "{";
	fout << "alphabet:";
	for (auto elem : node->alphabet)
		fout << elem << ",";
	fout << ";";
	fout << "bits:";
	for (auto elem : node->B)
		fout << elem;
	fout << ";";
	//if(node->)
	fout << "middle:" << node->middle << ";";
	if (node->Left != NULL) {
		fout << "Left:" << ",";
		wavelet_tree_dump(fout, node->Left);
	}

	if (node->Right != NULL) {
		fout << "Right:" << ",";
		wavelet_tree_dump(fout, node->Right);
	}
	fout << "}";
	
}

wavelet_tree wavelet_tree_from_file(std::istream& fin) {
	if (!fin) {
		cerr << "Error in opening the file" << endl;
		exit;
	}
	string str;
	getline(fin, str);
	//cout << str << "\n\n";

	return wavelet_tree(str, 0);

}


int main(int argc, char** argv) 
{ 
	if(argc<4){
		cout<< "Not al parameters given"<<endl;
		return 1;
	}

    std::string arg = argv[1];
	if(arg=="rank") {
		//$wt build <input string> <output file>
		std::string input = argv[2];
        std::string Indices= argv[3];
		std::string output = argv[4];

		ifstream infile (input);
        ifstream queryindices (Indices);
        
		ofstream outfile (output , std::ios_base::app | std::ios_base::out);
		outfile.setf(ios::fixed,ios::floatfield);

       // std::string bits("10101011101011111000");
        if (infile.is_open())
        {
			string bits;
			getline (infile,bits);
            bit_vector b;
            
            for (auto x : bits)
                b.push_back(x == '1');
            //cout<<b.at(1);
            rank_support r1(b);
            rank_support* r = &r1;

            //select_support s1(r);

            int a;
           // cout<<"Hello";

            if (queryindices.is_open())
            {
                //cout<<"a";
                auto start = std::chrono::system_clock::now();
                while (queryindices >> a) r1.rank1(a);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                outfile<<b.size()<<","<<elapsed_seconds.count()<<","<<r1.overhead()<<"\n";
            }
            else cout << "Unable to open file";

            outfile.close();
        }

        else cout << "Unable to open file";
    }

	if(arg=="select") {
		//$wt build <input string> <output file>
		std::string input = argv[2];
        std::string Indices= argv[3];
		std::string output = argv[4];

		ifstream infile (input);
        ifstream queryindices (Indices);
        
		ofstream outfile (output , std::ios_base::app | std::ios_base::out);
		outfile.setf(ios::fixed,ios::floatfield);

       // std::string bits("10101011101011111000");
        if (infile.is_open())
        {
			string bits;
			getline (infile,bits);
            bit_vector b;
            
            for (auto x : bits)
                b.push_back(x == '1');
            //cout<<b.at(1);
            rank_support r1(b);
            rank_support* r = &r1;

            select_support s1(r);

            int a;
           // cout<<"Hello";

            if (queryindices.is_open())
            {
                //cout<<"a";
                auto start = std::chrono::system_clock::now();
                while (queryindices >> a) s1.select1(a);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                outfile<<b.size()<<","<<elapsed_seconds.count()<<","<<r1.overhead()<<"\n";
            }
            else cout << "Unable to open file";

            outfile.close();
        }

        else cout << "Unable to open file";
    }

	if(arg=="wavelet") {
		//$wt build <input string> <output file>
		std::string input = argv[2];
        std::string Indices= argv[3];
		std::string output = argv[4];

		ifstream infile (input);
        ifstream queryindices (Indices);
        
		ofstream outfile (output , std::ios_base::app | std::ios_base::out);
		outfile.setf(ios::fixed,ios::floatfield);
		//cout<<"HERE";

       // std::string bits("10101011101011111000");
        if (infile.is_open())
        {
			string bits;
			getline (infile,bits);
            wavelet_tree wt(bits);
			//cout<<bits;

            char a;
			int b;
			
			
            if (queryindices.is_open())
            {
                //cout<<"a";
                auto start = std::chrono::system_clock::now();
                while (queryindices >> a>>b) wt.rank(a,b);

				
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
				queryindices.clear();
				queryindices.seekg(0, ios::beg);
				auto start2 = std::chrono::system_clock::now();
                while (queryindices >> a>>b) wt.select(a,b);
                auto end2 = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds2 = end-start;
				

                outfile<<wt.B.size()<<","<<wt.alphabet.size()<<","<<elapsed_seconds.count()<<","<<elapsed_seconds2.count()<<"\n";
            }

            else cout << "Unable to open file";

            outfile.close();
        }

        else cout << "Unable to open file";
    }
}
	
/*
int main(int argc, char** argv) 
{ 
	if(argc<3){
		cout<< "Not al parameters given"<<endl;
		return 1;
	}

    std::string arg = argv[1];
	if(arg=="build") {
		//$wt build <input string> <output file>
		std::string input = argv[2];
		std::string output = argv[3];

		ifstream infile (input);
		ofstream outfile (output);

		string bits;
  	
		if (infile.is_open())
		{
			
			getline (infile,bits);
			cout << bits << '\n';
			wavelet_tree wt(bits);
			if(outfile.is_open()){
				wavelet_tree_dump(outfile, &wt);
				outfile.close();
			}
			infile.close();
			cout<<"Number of distinct input characters:\t"<<wt.alphabet.size()<<"\n";
			cout<<"Number of characters in the input string:\t"<<bits.size()<<"\n";

		}
	}

	if(arg=="access"){
		std::string input = argv[2];
		std::string indices = argv[3];

		ifstream infile (input);
		ifstream queryindices (indices);
		if (infile.is_open())
		{
			wavelet_tree wt1 = wavelet_tree_from_file(infile);
			infile.close();
			int a;
			while (queryindices >> a)
			{
				cout<<wt1.access(a)<<"\n";
				// process pair (a,b)
			}
		}



	}



	if(arg=="rank"){
		std::string input = argv[2];
		std::string indices = argv[3];

		ifstream infile (input);
		ifstream queryindices (indices);
		if (infile.is_open())
		{
			wavelet_tree wt1 = wavelet_tree_from_file(infile);
			infile.close();
			char a;
			int b;
			while (queryindices >> a>>b)
			{
				cout<<wt1.rank(a,b)<<"\n";
				// process pair (a,b)
			}
		}

	}
    
	if(arg=="select"){
		std::string input = argv[2];
		std::string indices = argv[3];

		ifstream infile (input);
		ifstream queryindices (indices);
		if (infile.is_open())
		{
			wavelet_tree wt1 = wavelet_tree_from_file(infile);
			infile.close();
			char a;
			int b;
			while (queryindices >> a>>b)
			{
				cout<<wt1.select(a,b)<<"\n";
				// process pair (a,b)
			}
		}

	}
    return 0; 
} 



int main(int argc, char** argv) {

	std::string input = argv[2];
    std::string Indices= argv[3];
	std::string output = argv[4];

	string bits;
	ifstream infile(input);
	ifstream queryindices(Indices);
	ofstream outfile(output, std::ios_base::app | std::ios_base::out);

	
	if (infile.is_open())
	{
		getline (infile,bits);
		//cout << bits << '\n';

		bit_vector b;
		cout<<bits.length()<<"Hello"<<endl;

		for (auto a : bits)
			b.push_back(a == '1');
		//cout<<b.at(1000);
		rank_support r1(b);
		rank_support* r = &r1;

		select_support s1(r);
		int a;

		if (queryindices.is_open())
            {
                //cout<<"a";
                auto start = std::chrono::system_clock::now();
				while (queryindices >> a) cout<<r->rank1(a);
                auto end = std::chrono::system_clock::now();
                std::chrono::duration<double> elapsed_seconds = end-start;
                
				if (outfile.is_open())
				{
					outfile<<b.size()<<"\t"<<elapsed_seconds.count()<<"\t"<<r1.overhead()<<"\n";

					outfile.close();
				}
            }

		
		else cout << "Unable to open file";

		outfile.close();
	}
	/*

	else cout << "Unable to open file";
	

	//std::string bits("10101011101011111000");

	//wavelet_tree wt("helloworldagain");

	//cout << wt.rank('o', 6)<<"cc\n";
	//cout << wt.select('l', 3);

	//wavelet_tree_dump(outfile, &wt);
	//outfile.close();
	ifstream inputfile("output.txt");

	wavelet_tree wt1 = wavelet_tree_from_file(inputfile);
	
	std::string bits("1001011100001010");

	//bit_vector b(bits);

	bit_vector b;

	for (auto a : bits)
		b.push_back(a == '1');

	rank_support r1(b);
	rank_support* r = &r1;

	select_support s1(r);

	cout << "\n" << s1.select0(7);
	cout << "\n" << r1.rank1(13);

	r1.overhead();
	

	return 0;
}*/
