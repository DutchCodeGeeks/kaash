#include "builtins.h"

using namespace std;
extern VariableStore varstore;

unordered_map<string, function<int(vector<string>)>> builtins = {
	{ string("cd"), [] (vector<string> args) -> int {
		string loc;

		if (args.size() == 0 || args[0] == "~") {
			loc = varstore.get("HOME");
		} else {
			loc = args[0];
		}

		chdir(loc.c_str());
		return 0;
	}},
	{ string("exit"), [] (vector<string> args) -> int {
		int exitCode = 0;
		if ((int)args.size() > 0) {
			exitCode = stoi(args[0]);
		}

		exit(exitCode);
	}},
	{ string("alias"), [] (vector<string> args) -> int {
		if (args.size() != 2) {
			cerr << "expected 2 arguments" << endl;
			return 1;
		}
		string name = args[0];
		string def = args[1];

		return 0;
	}},
};

Maybe<int> callAndPrintFunction(const string &funcName, const vector<string> args) {
	auto func_it = builtins.find(funcName);
	if (func_it != builtins.end()) {
		return func_it->second(args);
	}
	return Nothing();
}
