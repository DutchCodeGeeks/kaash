#include "formatter.h"

using namespace std;

extern VariableStore varstore;

pair<char,int> backslashChar(const string &s,int start){
	char d1,d2;
	switch(s[start]){
		case '$':return {'$',1};break;
		case '0':return {'\0',1};break;
		case 'n':return {'\n',1};break;
		case 't':return {'\t',1};break;
		case 'r':return {'\r',1};break;
		case 'b':return {'\b',1};break;
		case 'v':return {'\v',1};break;
		case 'f':return {'\f',1};break;
		case 'x':
			if(s.size()-start<2)return {'\0',1};
			d1=s[start+1];
			if(d1>='a')d1=d1-'a'+10;
			else if(d1>='A')d1=d1-'A'+10;
			else if(d1>='0')d1=d1-'0';
			else d1=0;
			d2=s[start+2];
			if(d2>='a')d2=d2-'a'+10;
			else if(d2>='A')d2=d2-'A'+10;
			else if(d2>='0')d2=d2-'0';
			else d2=0;
			return {16*d1+d2,3};
		default:return {s[start],1};break;
	}
}

string formatString(string subj){
	stringstream ss;
	int i,j,length=subj.size();
	pair<char,int> bsResult;
	for(i=0;i<length;i++){
		const char c=subj[i];
		if(c=='\\'){
			bsResult=backslashChar(subj,i+1);
			ss<<bsResult.first;
			i+=bsResult.second;
			continue;
		}
		if(c!='$'){
			ss<<c;
			continue;
		}
		if(i+1==length)throw_error("loose dollar at end of string");
		if(subj[i+1]=='{'){ //variable expansion
			for(j=i+2;j<length&&subj[j]!='}';j++);
			if(j==length)throw_error("unterminated variable expansion");
			ss<<varstore.get(subj.substr(i+2,j-(i+2)));
			i=j;
		} else if(subj[i+1]=='('){
			for(j=i+2;j<length&&subj[j]!=')';j++);
			if(j==length)throw_error("unterminated string interpolation");
			const string cmd=subj.substr(i+2,j-(i+2));
			throw_error("string interpolation unimplemented!");
			//ss<<executeCommand(cmd);
			i=j;
		} else {
			throw_error("loose dollar in string");
		}
	}
	return ss.str();
}
