#include <errno.h>
#include <pwd.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include "builtins.h"

using namespace std;


string resolveTilde(const string &path){
	if(path.size()==0||path[0]!='~'){
		return path;
	}
	if(path.size()==1||path[1]=='/'){
		return getenv("HOME")+path.substr(1);
	}
	size_t slashidx=path.find('/',1);
	string name=path.substr(1,slashidx==string::npos ? string::npos : slashidx-1);
	struct passwd *pwd=getpwnam(name.data());
	if(!pwd){
		return path;
	}
	return pwd->pw_dir+path.substr(slashidx);
}

const unordered_map<string,Builtin> builtins={
	{"cd",[](const vector<string> &args, const unordered_map<int, int> &fdescs) -> i64 {
		string path;
		FILE *errf = fdopen(fdescs.at(2), "w");
		if(args.size()==1){
			path=resolveTilde("~");
		} else if(args.size()>=3){
			fprintf(errf, "Usage: %s [dir]\n", args[0].data());
			return 1;
		} else {
			path=resolveTilde(args[1]);
		}
		if(chdir(path.data())!=0){
			fprintf(errf, "cd: %s\n", strerror(errno));
			return 1;
		}
		return 0;
	}},
};
