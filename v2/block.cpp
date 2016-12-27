#include <algorithm>
#include <stdexcept>
#include <cassert>
#include "block.h"
#include "util.h"

using namespace std;


ParseError::ParseError(const string &what_arg)
	:runtime_error(what_arg){}
ParseError::ParseError(const char *what_arg)
	:runtime_error(what_arg){}


Redirect::Redirect()
	:isoutput(false),type(Redirect::Type::fd),fdval(-1){}

Redirect::Redirect(bool isoutput,int fdval)
	:isoutput(isoutput),type(Redirect::Type::fd),fdval(fdval){}

Redirect::Redirect(bool isoutput,const string &fileval)
	:isoutput(isoutput),type(Redirect::Type::file),fileval(fileval){}


Argument::Chunk::Chunk(const string &strval)
	:type(Type::string),strval(strval){}

Argument::Chunk::Chunk(string &&strval)
	:type(Type::string),strval(move(strval)){}

Argument::Chunk::Chunk(const Block &blockval)
	:type(Type::block),blockval(new Block(blockval)){}

Argument::Chunk::Chunk(Type type,const string &strval)
	:type(type),strval(strval){
	if(type!=Type::string&&type!=Type::variable){
		throw logic_error("Invalid Chunk type in combination with strval");
	}
}

Argument::Chunk::Chunk(const Chunk &other)
	:type(other.type),strval(other.strval){
	if(other.blockval){
		blockval=new Block(*other.blockval);
	}
}

Argument::Chunk::~Chunk(){
	if(blockval){
		delete blockval;
	}
}

Argument::Chunk& Argument::Chunk::operator=(const Chunk &other){
	type=other.type;
	strval=other.strval;
	if(other.blockval){
		blockval=new Block(*other.blockval);
	}
	return *this;
}


Argument::Argument(){}

Argument::Argument(const Chunk &chunk){
	chunks.push_back(chunk);
}


Block::Block(){}


// Returns index in `str` of closing ')'
// `start` should point at the opening '('
size_t findInterpolationClose(const string &str,const size_t start,const size_t size){
	i64 depth=1;
	size_t i=start+1;
	while(i<size&&depth>0){
		if(str[i]=='"'||str[i]=='\''){
			char quote=str[i];
			for(i++;i<size&&str[i]!=quote;i++){
				if(str[i]=='\\'){
					i++;
				}
			}
		}
		if(i+1<size&&str[i]=='$'&&str[i+1]=='('){
			depth++;
			i++;
		} else if(str[i]==')'){
			depth--;
		}
		i++;
	}
	if(depth>0){
		throw ParseError("Unterminated interpolation");
	}
	return i;
}


bool iswordchar(char c){
	return isalpha(c)||isdigit(c)||c=='_';
}


class Tokeniser{
	const string source;
	const i64 size;
	i64 index=0,nextat=0;

public:
	Tokeniser(const string &source)
			:source(source),size(source.size()){
		advance();
	}

	bool eof() const {
		return index==-1;
	}

	string get(){
		if(index==-1){
			throw logic_error("Tokeniser::get() after last token");
		}
		if(nextat==index){
			throw logic_error("Tokeniser::get() before advance(): index="+to_string(index));
		}
		return source.substr(index,nextat-index);
	}

	bool advance(){
		static const char *special="|";

		index=nextat;

		int previndex;
		do {
			previndex=index;

			while(index<size&&isspace(source[index])){
				if(source[index]=='\n'){
					nextat=index+1;
					return true;
				}
				index++;
			}
			if(index==size){
				return false;
			}

			if(source[index]=='\\'){
				if(index==size-1){
					throw ParseError("Backslash at end of file");
				}
				if(source[index+1]!='\n'){
					break;
				}
				index+=2;
			}

			if(source[index]=='#'){
				index++;
				while(index<size&&source[index]!='\n'){
					index++;
				}
				if(index==size){
					return false;
				}
			}
		} while(index!=previndex);
		assert(index<size);

		nextat=index;

		while(nextat<size){
			if(strchr(special,source[nextat])!=nullptr){
				if(nextat==index){
					nextat++;
				}
				break;
			} else if(isspace(source[nextat])){
				break;
			} else if(source[nextat]=='\\'){
				nextat++;
				if(nextat==size){
					throw ParseError("Backslash at end of file");
				}
				if(source[nextat]=='\n'){ //don't count that as part of a word
					nextat--;
					return true;
				}
				nextat++;
			} else if(source[nextat]=='"'||source[nextat]=='\''){
				char quote=source[nextat];
				nextat++;
				while(nextat<size&&source[nextat]!=quote){
					if(source[nextat]=='\\'){
						nextat++;
					}
					nextat++;
				}
				if(nextat>=size){
					throw ParseError("Unterminated string");
				}
				nextat++;
			} else if(source[nextat]=='$'){
				if(nextat+5<size&&source[nextat+1]=='.'&&source[nextat+2]=='.'&&
				   source[nextat+3]=='.'&&source[nextat+4]=='('){
					nextat=nextat+4;
				} else if(nextat+2<size&&source[nextat+1]=='('){
					nextat=nextat+1;
				} else {
					//variable name
					nextat=nextat+1;
				}

				if(source[nextat]=='('){
					//count parentheses
					nextat=findInterpolationClose(source,nextat,size);
				} else {
					//variable name
					while(nextat<size&&
					      !isspace(source[nextat])&&iswordchar(source[nextat])){
						nextat++;
					}
				}
			} else {
				nextat++; //normal character
			}
		}

		return true;
	}
};

int unhexchar(char c){
	if(c>='0'&&c<='9'){
		return c-'0';
	} else if(c>='a'&&c<='f'){
		return c-'a'+10;
	} else if(c>='A'&&c<='F'){
		return c-'A'+10;
	} else {
		throw ParseError("Invalid hexadecimal escape in string");
	}
}

string unescapeRaw(const string &str){
	string res;
	for(size_t i=0;i<str.size();i++){
		if(str[i]!='\\'){
			res+=str[i];
			continue;
		}

		i++;
		if(i==str.size()){
			throw logic_error("Unexpected backslash at end of string in unescapeRaw");
		}
		switch(str[i]){
			case 'n': res+='\n'; break;
			case 'r': res+='\r'; break;
			case 't': res+='\t'; break;

			case 'x':
				if(i+2>=str.size()){
					throw ParseError("Partial hexadecimal escape in string");
				}
				res+=(char)(16*unhexchar(str[i+1])+unhexchar(str[i+2]));
				i+=2;
				break;

			default:
				res+=str[i];
				break;
		}
	}
	return res;
}

Argument parseArgument(const string &src){
	Argument argument;
	for(size_t i=0;i<src.size();i++){
		if(argument.spreaded){
			throw ParseError("Spreaded interpolation can only stand alone as an argument");
		}
		if(src[i]=='\''||src[i]=='"'){
			char quote=src[i];
			size_t starti=i+1;
			for(i++;i<src.size()&&src[i]!=quote;i++){
				if(src[i]=='\\'){
					i++;
				}
			}
			if(i>=src.size()){
				throw logic_error("Expected closing \"'\" in argument string");
			}
			argument.chunks.emplace_back(unescapeRaw(src.substr(starti,i-starti)));
		} else if(src[i]=='$'){
			i++;
			if(src[i]=='('){
				size_t closeidx=findInterpolationClose(src,i,src.size());
				argument.chunks.emplace_back(Block(src.substr(i+1,closeidx-i-2)));
				i=closeidx;
			} else if(src[i]=='.'&&src[i+1]=='.'&&src[i+2]=='.'&&src[i+3]=='('){
				if(argument.chunks.size()>0){
					throw ParseError("Spreaded interpolation can only stand alone as an argument");
				}
				i+=3;
				size_t closeidx=findInterpolationClose(src,i,src.size());
				argument.chunks.emplace_back(Block(src.substr(i+1,closeidx-i-2)));
				i=closeidx;
				argument.spreaded=true;
			} else {
				size_t starti=i;
				while(i<src.size()&&iswordchar(src[i])){
					i++;
				}
				argument.chunks.emplace_back(Argument::Chunk::Type::variable,src.substr(starti,i-starti));
			}
		} else {
			size_t starti=i;
			for(;i<src.size()&&src[i]!='"'&&src[i]!='\''&&src[i]!='$';i++){
				if(src[i]=='\\'){
					i++;
				}
			}
			argument.chunks.emplace_back(unescapeRaw(src.substr(starti,i-starti)));
		}
	}

	for(size_t i=1;i<argument.chunks.size();i++){
		if(argument.chunks[i].type==Argument::Chunk::Type::string&&
		   argument.chunks[i-1].type==Argument::Chunk::Type::string){
			argument.chunks[i-1].strval+=argument.chunks[i].strval;
			argument.chunks.erase(argument.chunks.begin()+i);
			i--;
		}
	}

	return argument;
}

Call parseCall(const vector<string> &tokens){
	Call callvec;
	for(const string &token : tokens){
		assert(token.size()>0);

		bool isredirect=false;
		bool isoutput;
		int fdkey;
		size_t subjectidx;
		if(token[0]=='<'){
			isredirect=true;
			isoutput=false;
			fdkey=0;
			subjectidx=1;
		} else if(token[0]=='>'){
			isredirect=true;
			isoutput=true;
			fdkey=1;
			subjectidx=1;
		} else if(isdigit(token[0])){
			size_t i;
			for(i=1;i<token.size();i++){
				if(!isdigit(token[i]))break;
			}
			if(i!=token.size()&&(token[i]=='<'||token[i]=='>')){
				isredirect=true;
				isoutput=token[i]=='<';
				try {
					fdkey=stoi(token);
				} catch(out_of_range){
					throw ParseError("Redirect file descriptor out of range");
				}
				subjectidx=i+1;
			}
		}

		if(!isredirect){
			callvec.arguments.push_back(parseArgument(token));
		} else {
			if(subjectidx>=token.size()){
				throw ParseError("Redirect source/target expected");
			}

			Redirect redir;
			redir.isoutput=isoutput;
			if(token[subjectidx]=='&'){
				redir.type=Redirect::Type::fd;
				try {
					size_t len;
					redir.fdval=stoi(token.substr(subjectidx+1),&len);
					if(token.size()!=subjectidx+1+len){
						throw ParseError("Extra characters after '&' redirect file descriptor");
					}
				} catch(invalid_argument){
					throw ParseError("File descriptor number expected after '&' in redirect");
				} catch(out_of_range){
					throw ParseError("Redirect '&' file descriptor out of range");
				}
			} else {
				redir.type=Redirect::Type::file;
				redir.fileval=token.substr(subjectidx);
			}

			callvec.redirects.emplace_back(fdkey,redir);
		}
	}
	return callvec;
}

Block::Block(const string &source){
	// cerr<<"Calling block constructor with source: <<<"<<source<<">>>"<<endl;
	Tokeniser tokeniser(source);
	if(tokeniser.eof()){
		return;
	}
	vector<Call> pipeline;
	bool eof=false;
	while(!eof){
		vector<string> tokens;
		string token;
		while(true){
			token=tokeniser.get();
			// cerr<<"<["<<token<<"]>"<<endl;
			if(token=="\n"||token=="|"){
				if(!tokeniser.advance()){
					eof=true;
				}
				break;
			}
			tokens.push_back(token);
			if(!tokeniser.advance()){
				token="\n";
				eof=true;
				break;
			}
		}

		if(tokens.size()==0){
			if(pipeline.size()==0){
				continue;
			} else {
				throw ParseError("Empty call in pipeline");
			}
		}
		pipeline.push_back(parseCall(tokens));
		if(token=="\n"){
			lines.push_back(move(pipeline));
			pipeline.clear();
		}
	}
}


string escapedString(const string &str){
	string res;
	for(unsigned char c : str){
		switch(c){
			case '\n': res+="\\n"; break;
			case '\r': res+="\\r"; break;
			case '\t': res+="\\t"; break;
			default:
				if(c<32||c>=127){
					res+="\\x";
					res+="0123456789abcdef"[c/16];
					res+="0123456789abcdef"[c%16];
				} else {
					res+=c;
				}
		}
	}
	return res;
}

ostream& operator<<(ostream &os,const Block &block);

ostream& operator<<(ostream &os,const Argument &argument){
	if(argument.spreaded){
		os<<"[[spread] ";
	}
	for(const Argument::Chunk &chunk : argument.chunks){
		switch(chunk.type){
			case Argument::Chunk::Type::string:
				os<<'\''<<escapedString(chunk.strval)<<'\'';
				break;

			case Argument::Chunk::Type::block:
				os<<"$("<<*chunk.blockval<<')';
				break;

			case Argument::Chunk::Type::variable:
				os<<"$"<<chunk.strval;
				break;
		}
	}
	if(argument.spreaded){
		os<<']';
	}
	return os;
}

ostream& operator<<(ostream &os,const Call &call){
	bool first=true;
	for(const Argument &arg : call.arguments){
		if(first){
			first=false;
		} else {
			os<<' ';
		}
		os<<arg;
	}
	return os;
}

ostream& operator<<(ostream &os,const Block &block){
	for(size_t i=0;i<block.lines.size();i++){
		bool first=true;
		for(const Call &call : block.lines[i]){
			if(first){
				first=false;
			} else {
				os<<" | ";
			}
			os<<call;
		}
		if(i!=block.lines.size()-1){
			os<<endl;
		}
	}
	return os;
}
