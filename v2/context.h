#pragma once

#include <string>
#include <unordered_map>
#include "block.h"
#include "maybe.h"
#include "util.h"

using namespace std;


class Value{
public:
	enum class Type{
		string,
		function,
	};

	Type type;
	string strval;
	Block funval;

	Value(const string &s);
	Value(const Block &bl);
};

class Varstore{
	unordered_map<string,Value> variables;

public:
	Maybe<string> getstring(const string &name);
	Maybe<Block> getfunction(const string &name);

	void setstring(const string &name,const string &value);
};

class Context{
	Pipeline preparePipeline(const Pipeline &origline);

public:
	Varstore varstore;

	i64 execute(const Pipeline &line,string *outputcollect=nullptr);
	i64 execute(const Block &block,string *outputcollect=nullptr);
};
