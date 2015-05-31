#pragma once

#include <sstream>
#include "util.h"
#include "variables.h"

using namespace std;

pair<char,int> backslashChar(const string &s, int start=0);
string formatString(string subj);
