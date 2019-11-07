//#define _GLIBCXX_USE_CXX11_ABI 0
#include"wavelet.h"
#include<string>

using std::string;
wavelet_tree::wavelet_tree(string str) {
	
		std::string temp_left = "";
		std::string temp_right = "";
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

	wavelet_tree::wavelet_tree(std::string str, int offset) {
		
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

		if (str.substr(offset).find_first_not_of('}') == std::string::npos) return;
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

	int wavelet_tree::rank(char c, int index) {
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

	int wavelet_tree::select(char c, int occurrence) {
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


	char wavelet_tree::access(int index) {
		
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
	