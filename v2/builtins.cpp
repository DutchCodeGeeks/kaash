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
	{"cd",[](const vector<string> &args) -> i64 {
		string path;
		if(args.size()==1){
			path=resolveTilde("~");
		} else if(args.size()>=3){
			cerr<<"Usage: "<<args[0]<<" [dir]"<<endl;
			return 1;
		} else {
			path=resolveTilde(args[1]);
		}
		if(chdir(path.data())!=0){
			cerr<<"cd: "<<strerror(errno)<<endl;
			return 1;
		}
		return 0;
	}},
};
