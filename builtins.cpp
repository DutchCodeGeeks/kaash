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
	{ string("pwd"), [] (vector<string> args) {
		return varstore.get("PWD");
	}},
	{ string("exit"), [] (vector<string> args) {
		int exitCode = 0;
		if ((int)args.size() > 0) {
			exitCode = stoi(args[0]);
		}

		exit(exitCode);
		return "";
	}},
	{ string("echo"), [] (vector<string> args) {
		if (args.size() == 0) {
			return string("");
		} else {
			return joinVector(args, " ");
		}
	}},
	{ string("ls"), [] (vector<string> args) {
		string loc;
		stringstream ss;

		if (args.size() == 0) {
			loc = ".";
		} else {
			loc = args[0];
		}

		Maybe<vector<dirEntry>> mdir = readDir(loc);
		if (mdir.isNothing()) {
			return args[0];
		}
		const vector<dirEntry> &dir = mdir.fromJust();
		for (unsigned int i = 0; i < dir.size(); i++) {
			const dirEntry &entry = dir[i];

			if (i != 0) ss << endl;
			ss << entry.name;
		}

		return ss.str();
	}},
};

void callAndPrintFunction(const string &funcName, const vector<string> args) {
	auto func_it = builtins.find(funcName);
	if (func_it != builtins.end()) {
		string res = func_it->second(args);
		if (res.size() > 0) {
			cout << res << endl;
		}
	}
}
