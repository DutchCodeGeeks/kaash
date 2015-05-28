#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include "formatter.h"
#include "variables.h"

using namespace std;

extern VariableStore varstore;


bool repl(void){
	char *line = readline(formatString(varstore.get("kaash_prompt")).c_str());

	if (line) {
		cout << line << endl;
		add_history(line);
		return true;
	}
	return false;
}


int main(void){
	rl_bind_key('\t', rl_complete);

	varstore.store("kaash_prompt","${TIME} ${PWD}\\$ ");
	while(repl());
	return 0;
}
