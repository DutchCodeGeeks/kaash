#include <sstream>
#include "formatter.h"
#include "util.h"
#include "variables.h"

using namespace std;

extern VariableStore varstore;

string formatString(string subj){
	stringstream ss;
	int i,j,length=subj.size();
	for(i=0;i<length;i++){
		const char c=subj[i];
		if(c=='\\'){
			switch(subj[i+1]){
				case '$':ss<<'$';break;
				case 'n':ss<<'\n';break;
				case 't':ss<<'\t';break;
				case 'r':ss<<'\r';break;
				case 'b':ss<<'\b';break;
				case 'v':ss<<'\v';break;
				default:ss<<subj[i+1];break;
			}
			i++;
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
