#include <iostream>
#include <fstream>
#include <cstring>
//#include <readline/readline.h>
//#include <readline/history.h>
#include "block.h"
#include "context.h"
#include "util.h"

using namespace std;


int main(int argc,char **argv){
	//rl_bind_key('\t',rl_complete);
	//rl_set_signals();

	Context context;

	for(int i=1;i<argc;i++){
		string contents;
		if(strcmp(argv[i],"-")==0){
			contents=readstream(cin);
		} else {
			ifstream file(argv[i]);
			if(!file){
				cerr<<"Cannot open file '"<<argv[i]<<"'"<<endl;
				return 1;
			}
			contents=readstream(file);
		}
		
		context.execute(Block(contents));
	}
}
