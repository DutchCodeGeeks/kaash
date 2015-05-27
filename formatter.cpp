#include <sstream>
#include "formatter.h"
#include "util.h"
#include "variables.h"

using namespace std;

extern unordered_map<string,string> variables;


string formatString(string subj){
	stringstream ss;
	int i,j,length=subj.size();
	for(i=0;i<length;i++){
		const char c=subj[i];
		if(subj[i]!='$'){
			ss<<c;
			continue;
		}
		if(i+1==length)throw_error("loose dollar at end of string");
		if(subj[i+1]=='{'){ //variable expansion
			for(j=i+2;j<length&&subj[j]!='}';j++);
			if(j==length)throw_error("unterminated variable expansion");
			ss<<getVariable(subj.substr(i+2,j-(i+2)));
		} else if(subj[i+1]=='('){
			for(j=i+2;j<length&&subj[j]!=')';j++);
			if(j==length)throw_error("unterminated string interpolation");
			const string cmd=subj.substr(i+2,j-(i+2));
			//ss<<executeCommand(cmd);
		}
	}
}
