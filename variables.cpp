#include "variables.h"
#include <set>
#include <vector>

using namespace std;

unordered_map<string,string> variables;
vector<set<string>> scopes;
set<string> *currentScope=NULL;

string getVariable(string name){
	unordered_map<string,string>::const_iterator it=variables.find(name);
	if(it==variables.cend())return "";
	return it->second;
}

bool variableExists(string name){
	unordered_map<string,string>::const_iterator it=variables.find(name);
	return it!=variables.cend();
}

void setVariable(string name,string val){
	if(currentScope&&currentScope->find(name)==currentScope->end()){
		currentScope->insert(name);
	}
	variables[name]=val;
}


void enterScope(void){
	scopes.emplace_back();
	currentScope=&scopes[scopes.size()-1];
}

void leaveScope(void){
	for(const string &name : *currentScope){
		variables.erase(variables.find(name));
	}
	scopes.pop_back();
	currentScope=&scopes[scopes.size()-1];
}
