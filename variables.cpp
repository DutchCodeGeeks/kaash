#include <cstring>
#include "variables.h"

using namespace std;

extern const char *const *const environ; //set in the c library - magic; $ man environ

VariableStore varstore; //for use with `extern`

VariableStore::VariableStore(void):
	electricVars({
		{string("DATE"),[](void){
			time_t tim=time(NULL);
			struct tm *timeinfo=localtime(&tim);
			char buf[64];
			strftime(buf,63,"%D",timeinfo);
			return string(buf);
		}},
		{string("TIME"),[](void){
			time_t tim=time(NULL);
			struct tm *timeinfo=localtime(&tim);
			char buf[64];
			strftime(buf,63,"%T",timeinfo);
			return string(buf);
		}},
		{string("PWD"),[](void){
			char *buf=getcwd(NULL,0);
			string s=buf;
			free(buf);
			return s;
		}},
		{string("KAASH"),[](void){
			return "kaas.";
		}}
	}){
	const char *envvar=*environ;
	const char *found;
	int i;
	for(i=0;envvar;envvar=environ[++i]){
		found=strchr(envvar,'=');
		if(!found)continue; //no '=' in the line, wtf?
		store(string(envvar,(int)(found-envvar)),string(found+1));
	}
}

string VariableStore::get(string name){
	map<string,function<string(void)>>::const_iterator elec_it=electricVars.find(name);
	if(elec_it!=electricVars.cend())return elec_it->second();

	unordered_map<string,string>::const_iterator vars_it=variables.find(name);
	if(vars_it==variables.cend())return "";
	return vars_it->second;
}

bool VariableStore::exists(string name){
	unordered_map<string,string>::const_iterator it=variables.find(name);
	return it!=variables.cend();
}

void VariableStore::store(string name,string val){
	if(currentScope&&currentScope->find(name)==currentScope->end()){
		currentScope->insert(name);
	}
	variables[name]=val;
}


void VariableStore::enterScope(void){
	scopes.emplace_back();
	currentScope=&scopes[scopes.size()-1];
}

void VariableStore::leaveScope(void){
	for(const string &name : *currentScope){
		variables.erase(variables.find(name));
	}
	scopes.pop_back();
	if(scopes.size()!=0)currentScope=&scopes[scopes.size()-1];
}


