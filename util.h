#pragma once

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <dirent.h>
#include "maybe.h"

using namespace std;

void throw_error(string);

vector<string> split(const string&, const char, int numsplits = -1);

void readFile(const char* const filePath, function<void(string)> func);

string joinVector(const vector<string> &vect, const string &seperator);
string joinVector(const vector<string> &vect, char seperator);

enum dirEntryType {
	UNKNOWN=0,
	FIFO=1,
	CHR=2,
	DIRECTORY=4,
	BLK=6,
	REG=8,
	LNK=10,
	SOCK=12,
	WHT=14
};

struct dirEntry {
	dirEntryType type;
	string name;
};

Maybe<vector<dirEntry>> readDir(const string &dirname);
