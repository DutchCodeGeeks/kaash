#include "builtins.h"

using namespace std;
extern VariableStore varstore;

unordered_map<string, function<string(vector<string>)>> builtins = {
	{ string("cd"), [] (vector<string> args) {
		string loc;

		if (args.size() == 0 || args[0] == "~") {
			loc = varstore.get("HOME");
		} else {
			loc = args[0];
		}

		chdir(loc.c_str());
		return string("");
	}},
	{ string("exit"), [] (vector<string> args) {
		int exitCode = 0;
		if ((int)args.size() > 0) {
			exitCode = stoi(args[0]);
		}

		exit(exitCode);
		return "";
	}},
	{ string("alias"), [] (vector<string> args) {
		if (args.size() != 2) {
			return string("expected 2 arguments");
		}
		string name = args[0];
		string def = args[1];

		return string("");
	}},
};

bool callAndPrintFunction(const string &funcName, const vector<string> args) {
	auto func_it = builtins.find(funcName);
	if (func_it != builtins.end()) {
		string res = func_it->second(args);
		if (res.size() > 0) {
			cout << res << endl;
		}
		return true;
	}
	return false;
}
