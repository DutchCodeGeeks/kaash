#include <iostream>
#include <cstdlib>
#include <readline/readline.h>
#include <readline/history.h>
#include "formatter.h"
#include "variables.h"

using namespace std;

extern VariableStore varstore;


//puts '\0' after last non-whitespace, returns pointer to first non-whitespace
//doesn't do any reallocating, `free` still works normally, obviously
char* stripCstringInPlace(char *s){
	char *start,*end;

	for(start=s;isspace(*start);start++);
	if(!*start)return start;

	for(end=s+strlen(s)-1;end>start&&isspace(*end);end--);
	*(end+1)='\0';
	return start;
}


bool repl(void){
	char *c_line=readline(formatString(varstore.get("kaash_prompt")).c_str());

	if(!c_line)return false;
	const char *stripped=stripCstringInPlace(c_line);
	if(!*stripped)return true; //empty line; first char is '\0'
	add_history(stripped);
	string line=stripped;
	free(c_line);
	cout<<line<<endl;
	if(line=="exit")return false;
	return true;
}


int main(void){
	rl_bind_key('\t', rl_complete);

	rl_set_signals();

	varstore.store("kaash_prompt","${TIME} ${PWD}\\$ ");
	while(repl());
	return 0;
}
