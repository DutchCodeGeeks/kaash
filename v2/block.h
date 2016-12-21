#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace std;


class ParseError : public runtime_error{
public:
	ParseError(const string &what_arg);
	ParseError(const char *what_arg);
};


class Redirect{
public:
	enum class Type{
		fd,
		file,
	};

	bool isoutput;

	Type type;
	int fdval;
	string fileval;

	Redirect();
	Redirect(bool isoutput,int fdval);
	Redirect(bool isoutput,const string &fileval);
};

class Block;

class Argument{
public:
	class Chunk{
	public:
		enum class Type{
			string,
			block,
			variable,
		};

		Type type;
		string strval; // also dubs as variable name
		Block *blockval=nullptr;

		explicit Chunk(const string &strval);
		explicit Chunk(string &&strval);
		explicit Chunk(const Block &blockval);
		Chunk(Type type,const string &strval);
		Chunk(const Chunk &other);
		~Chunk();

		Chunk& operator=(const Chunk &other);
	};

	bool spreaded=false;
	vector<Chunk> chunks;

	Argument();
	explicit Argument(const Chunk &chunk);
};

class Call{
public:
	vector<Argument> arguments;
	vector<pair<int,Redirect>> redirects;
};

using Pipeline = vector<Call>;

class Block{
public:
	vector<Pipeline> lines;

	Block();
	Block(const string &source); //parsing constructor
};

ostream& operator<<(ostream &os,const Argument &argument);
ostream& operator<<(ostream &os,const Call &call);
ostream& operator<<(ostream &os,const Block &block);
