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
private:
	unordered_map<string,string> variables;
	vector<set<string>> scopes;
	set<string> *currentScope=NULL;
	map<string,function<string(void)>> electricVars;
	void store(const string &name, function<string(void)>);
public:
	VariableStore(void);
	string get(string name);
	bool exists(string name);
	void store(const string &name, const string &val);

	void enterScope(void);
	void leaveScope(void);
};
