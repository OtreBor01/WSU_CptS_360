#include "Header.h"

int main(void) {
	ifstream infile ("../inputs/input_8_3.txt");
	vector<int> numbers;
	int value = 0;
	if (infile.is_open()) {
		while (!infile.eof()) {
			infile >> value;
			numbers.push_back(value); //Checks for an empty character that might of been read in
		}
		cout << "Vector Size: " << numbers.size() - 1 << endl << endl;
		cout << "***** One *****" << endl << endl;
		clock_t time = clock();
		clock_t start;
		do start = clock();
		while (start == time);
		int sum = maxSubSum1(numbers);
		clock_t end = clock();
		double seconds = (double)(end - start) / CLOCKS_PER_SEC;
		double microseconds = (double)(end - start) / CLOCKS_PER_SEC * MICRO;
		cout << "maxSubSum1: " << sum << endl;
		cout << "CPU elapsed time in Microseconds: " << microseconds << endl << endl;
		cout << "CPU elapsed time in Seconds: " << seconds << endl << endl;
		
		cout << "***** Two *****" << endl << endl;
		time = clock();
		do start = clock();
		while (start == time);
		sum = maxSubSum2(numbers);
		end = clock();
		seconds = (double)(end - start) / CLOCKS_PER_SEC;
		microseconds = (double)(end - start) / CLOCKS_PER_SEC * MICRO;
		cout << "maxSubSum2: " << sum << endl;
		cout << "CPU elapsed time in Microseconds: " << microseconds << endl << endl;
		cout << "CPU elapsed time in Seconds: " << seconds << endl << endl;

		cout << "***** Three *****" << endl << endl;
		time = clock();
		do start = clock();
		while (start == time);
		sum = maxSubSum3(numbers);
		end = clock();
		seconds = (double)(end - start) / CLOCKS_PER_SEC;
		microseconds = (double)(end - start) / CLOCKS_PER_SEC * MICRO;
		cout << "maxSubSum3: " << sum << endl;
		cout << "CPU elapsed time in Microseconds: " << microseconds << endl << endl;
		cout << "CPU elapsed time in Seconds: " << seconds << endl << endl;
		
		cout << "***** Four *****" << endl << endl;
		time = clock();
		do start = clock();
		while (start == time);
		sum = maxSubSum4(numbers);
		end = clock();
		seconds = (double)(end - start) / CLOCKS_PER_SEC;
		microseconds = (double)(end - start) / CLOCKS_PER_SEC * MICRO;
		cout << "maxSubSum4: " << sum << endl;
		cout << "CPU elapsed time in Microseconds: " << microseconds << endl << endl;
		cout << "CPU elapsed time in Seconds: " << seconds << endl << endl;
		
		infile.close();
	}else {
		cout << "ERROR: File Failed to Open" << endl;
	}
	return 0;
}