#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <functional>

using namespace std;

void throw_error(string);

string cstr_to_str(char*);

vector<string> split(const string&, const char, int numsplits = -1);

void readFile(const char* const filePath, function<void(string)> func);

string joinVector(const vector<string> &vect, const string &seperator);
string joinVector(const vector<string> &vect, char seperator);
