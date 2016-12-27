#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "util.h"

using namespace std;


using Builtin = function<i64(const vector<string>&,const unordered_map<int,int>&)>;

extern const unordered_map<string,Builtin> builtins;
