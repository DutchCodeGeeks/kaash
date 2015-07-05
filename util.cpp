#include "util.h"

using namespace std;

void throw_error(string s){
	throw "Error: "+s;
}

string cstr_to_str(char *c_str) {
	string str = c_str;
	free(c_str);
	return string(str);
}

vector<string> split(const string &s, const char delim, int numsplits) {
	string::size_type cursor = 0;
	string::size_type idx = s.find(delim);
	vector<string> vec;
	while (idx != string::npos && numsplits != 0) {
		vec.push_back(s.substr(cursor, idx - cursor));
		numsplits--;
		cursor = idx + 1;
		idx = s.find(delim, cursor);
	}
	vec.push_back(s.substr(cursor, string::npos));
	return vec;
}

void readFile(const char* const filePath, function<void(string)> func) {
	ifstream f(filePath);

	if (!!f) {
		string line;
		while (f) {
			getline(f, line);
			func(line);
		}
	} else {
		char *message;
		asprintf(&message, "Can't read file '%s'. Does it exist?", filePath);
		throw message;
	}
}

string joinVector(const vector<string> &vect, const string &seperator) {
	string s;
	for (int i = 0; i < (int)vect.size(); i++) {
		s += vect[i];
		if (i != (int)vect.size() - 1) s += seperator;
	}
	return s;
}

string joinVector(const vector<string> &vect, char seperator) {
	return joinVector(vect, string(1, seperator));
}

vector<dirEntry> readDir(const string &dirname) {
	vector<dirEntry> result;
	auto dir = opendir(dirname.c_str());
	dirent *item = readdir(dir);
	while (item != NULL) {
		dirEntry entry;
		entry.name = string(item->d_name);
		entry.type = static_cast<dirEntryType>(item->d_type);
		result.push_back(entry);

		item = readdir(dir);
	}
	return result;
}
