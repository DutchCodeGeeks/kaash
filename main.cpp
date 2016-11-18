#include <cstdlib>
#include <errno.h>
#include <cstring>
#include <iostream>
#include <numeric>
#include <readline/history.h>
#include <readline/readline.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include "formatter.h"
#include "variables.h"
#include "builtins.h"
#include "util.h"
#include "maybe.h"

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

Maybe<int> callCommandSync(string name, vector<string> args) { // -> exitcode
	int pipefd[2];
	if (pipe(pipefd) == -1) {
		throw_error("error calling pipe(2)");
	}

	if (callAndPrintFunction(name, args).isNothing()) {
		pid_t pid = fork();
		if (pid == -1) {
			throw_error("can't fork");
		} else if (pid == 0) {
			char *cargs[args.size() + 2];
			cargs[0] = (char*) name.c_str();

			// put all the arguments from the `args` vector in the `cargs` array
			accumulate(args.begin(), args.end(), cargs + 1, [](char **cargs, const string &arg) {
				*cargs = (char*) arg.c_str();
				return cargs + 1;
			});

			cargs[args.size() + 1] = NULL;

			execvp(name.c_str(), cargs);

			int er = errno;
			write(pipefd[1], &er, sizeof(int));
			exit(0);
		} else {
			int status;
			while (true) {
				waitpid(pid, &status, 0);
				if (WIFEXITED(status)) {
					break;
				}
			}

			fd_set rdset;
			FD_ZERO(&rdset);
			FD_SET(pipefd[0], &rdset);
			struct timeval timeout = {.tv_sec = 0, .tv_usec = 0};
			int ret = select(pipefd[0] + 1, &rdset, NULL, NULL, &timeout);
			if (ret < 0) {
				throw_error("error calling select(2)");
			} else if (ret == 0) {
				return WEXITSTATUS(status);
			} else {
				int er;
				read(pipefd[0], &er, sizeof(int));
				if (er == ENOENT) {
					throw_error("Command '" + name + "' not found");
				} else if (er == EACCES) {
					throw_error("'" + name + "' not executable");
				}
			}
		}
	}
	return Nothing();
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

	try {
		callCommandSync(splitted[0], args);
	} catch (string err) {
		cerr << err << endl;
	}


	return true;
}

int main(void){
	rl_bind_key('\t', rl_complete);
	rl_set_signals();

	varstore.store("kaash_prompt","${TIME} ${PWD}\\$ ");
	while(repl());
	return 0;
}
