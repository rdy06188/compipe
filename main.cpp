#include "benchmark.h"
#include <string>
#include <iostream>

using namespace std;

const string folder = "../datasets/";

int main(int argc,char *argv[]) {
	if (argc == 1){
		cout << "no dataset input" << endl;
		return 0;
	}
	cout<<argv[1]<<endl;
	cout << endl << "**Benchmark**" << endl << endl;
	uint32_t K = 100;
   
	cin >> K;
	BenchAllFlowSize((folder + argv[1]).c_str());
	BenchHH((folder + argv[1]).c_str());
}
