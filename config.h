#pragma once

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include "util.h"

using namespace std;

class Config{
private:
	unordered_map<string, string> configs;
public:
	Config(void);
};
