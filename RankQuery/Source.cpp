#include<iostream>
#include <fstream>
#include "Wavelet/wavelet.h"
#include<string>
using namespace std;

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
	std::string str;
	getline(fin, str);
	//cout << str << "\n\n";

	return wavelet_tree(str, 0);

}

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

		std::string bits;
  	
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

/*

int main(void) {
	//string bits;
	//ifstream infile("input.txt");
	//ofstream outfile("output.txt");
	/*
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

		if (outfile.is_open())
		{
			outfile << "\n" << s1.select0(7777);
			outfile << "\n" << r1.rank1(1333);
			outfile<<"\n"<<r1.overhead();

			outfile.close();
		}
		else cout << "Unable to open file";

		outfile.close();
	}

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
	/*
	std::vector<bool > bit_vector;

	bit_vector.push_back(false);
	std::vector<bool >::reference ref1 = bit_vector.at(0);
	cout << ref1 << endl; // ref1 implicitly cast to bool

	

	return 0;
}
*/