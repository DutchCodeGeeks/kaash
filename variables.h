#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <vector>
#include <functional>
#include <cstdlib>
#include <unistd.h>

using namespace std;


class VariableStore {
	unordered_map<string,string> variables;
	vector<set<string>> scopes;
	set<string> *currentScope=NULL;
	map<string,function<string(void)>> electricVars;
public:
	VariableStore(void);
	string get(string name);
	bool exists(string name);
	void store(string name,string val);

	void enterScope(void);
	void leaveScope(void);
};
