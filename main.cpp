#include <iostream>
#include "formatter.h"
#include "variables.h"

using namespace std;

extern VariableStore varstore;


bool repl(void){
	string line;
	cout<<formatString(varstore.get("kaash_prompt"))<<flush;
	getline(cin,line);
	cout<<line<<endl;
	return true;
}


int main(void){
	varstore.store("kaash_prompt","${TIME} ${PWD}\\$ ");
	while(repl());
	return 0;
}
