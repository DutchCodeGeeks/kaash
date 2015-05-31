#include <cstdlib>
#include <iostream>
#include <readline/history.h>
#include <readline/readline.h>
#include "formatter.h"
#include "variables.h"
#include "builtins.h"
#include "util.h"

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

	vector<string> splitted = split(line, ' ');
	vector<string> args;
	for (auto it = splitted.begin() + 1; it != splitted.end(); ++it) {
		args.push_back(*it);
	}

	callAndPrintFunction(splitted[0], args);

	return true;
}


int main(void){
	rl_bind_key('\t', rl_complete);

	// rl_set_signals();

	varstore.store("kaash_prompt","${TIME} ${PWD}\\$ ");
	while(repl());
	return 0;
}
