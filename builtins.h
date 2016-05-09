#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <unordered_map>
#include <iostream>
#include "variables.h"
#include "util.h"
#include "maybe.h"

Maybe<int> callAndPrintFunction(const string&, const vector<string>);
