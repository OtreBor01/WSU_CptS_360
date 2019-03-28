#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>
#define MICRO 1000000
using namespace std;

int maxSubSum1(const vector<int> & a);
int maxSubSum2(const vector<int> & a);
int maxSubSum3(const vector<int> & a);
int maxSumRec(const vector<int> & a, int left, int right);
int maxSubSum4(const vector<int> & a);
int max3(int a, int b, int c);