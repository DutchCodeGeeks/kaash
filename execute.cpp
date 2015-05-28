#include <sstream>
#include <cstring>
#include <cctype>
#include "execute.h"
#include "variables.h"
#include "formatter.h"
#include "util.h"

extern VariableStore varstore;

enum TokenType {
	TT_WORD,
	TT_SYMBOL
};

struct Token {
	TokenType type;
	string val;
};

vector<Token> tokeniseCommand(const string &s){
	int i,len=s.size();
	char stringmode='\0';
	pair<char,int> bsResult;
	vector<Token> tkns;
	ostringstream wordss,ss;
	bool pushedstuff=false;
	for(i=0;i<len;i++){
		if(s[i]=='"'||s[i]=='\''){
			if(stringmode==s[i]){
				if(stringmode=='"')wordss<<formatString(ss.str());
				else wordss<<ss.str();
				ss.str("");
				stringmode='\0';
				pushedstuff=true; //even if ""; user might explicitly want an empty string
			} else {
				wordss<<formatString(ss.str());
				ss.str("");
				stringmode=s[i];
			}
			continue;
		}
		if(s[i]=='\\'){
			if(i+1==len)throw_error("loose backslash at end of string");
			bsResult=backslashChar(s,i+1);
			ss<<bsResult.first;
			pushedstuff=true;
			i+=bsResult.second;
			continue;
		}
		if(stringmode){
			ss<<s[i];
		} else {
			if(isspace(s[i])){
				if(pushedstuff){
					wordss<<formatString(ss.str());
					ss.str("");
					tkns.push_back({TT_WORD,wordss.str()});
					wordss.str("");
				}
			} else if(strchr(">^<|",s[i])!=NULL){
				if(pushedstuff){
					wordss<<formatString(ss.str());
					ss.str("");
					tkns.push_back({TT_WORD,wordss.str()});
					wordss.str("");
				}
				tkns.push_back({TT_SYMBOL,string(&s[i],1)});
			} else {
				ss<<s[i];
			}
		}
	}
	if(stringmode)throw_error("unterminated string");
	wordss<<formatString(ss.str());
	tkns.push_back({TT_WORD,wordss.str()});
	return tkns;
}
