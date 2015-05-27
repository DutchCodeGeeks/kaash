#pragma once

#include <string>
#include <unordered_map>

using namespace std;

string getVariable(string name);
bool variableExists(string name);
void setVariable(string name,string val);

void enterScope(void);
void leaveScope(void);
