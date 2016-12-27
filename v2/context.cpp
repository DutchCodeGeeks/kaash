#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include "builtins.h"
#include "context.h"

using namespace std;


Value::Value(const string &s)
	:type(Type::string),strval(s){}

Value::Value(const Block &bl)
	:type(Type::function),funval(bl){}


Maybe<string> Varstore::getstring(const string &name){
	auto it=variables.find(name);
	if(it!=variables.end()&&it->second.type==Value::Type::string){
		return it->second.strval;
	}
	char *envvar=getenv(name.data());
	if(envvar){
		return string(envvar);
	} else {
		return {};
	}
}

Maybe<Block> Varstore::getfunction(const string &name){
	auto it=variables.find(name);
	if(it!=variables.end()&&it->second.type==Value::Type::function){
		return it->second.funval;
	} else {
		return {};
	}
}


Pipeline Context::preparePipeline(const Pipeline &origline){
	Pipeline resline;
	for(const Call &origcall : origline){
		Call rescall;
		rescall.redirects=origcall.redirects;
		for(const Argument &arg : origcall.arguments){
			string result;
			for(const Argument::Chunk &chunk : arg.chunks){
				switch(chunk.type){
					case Argument::Chunk::Type::string:
						result+=chunk.strval;
						break;

					case Argument::Chunk::Type::variable: {
						Maybe<string> mvalue=varstore.getstring(chunk.strval);
						if(mvalue.isJust()){
							result+=mvalue.fromJust();
						}
						break;
					}

					case Argument::Chunk::Type::block: {
						string output;
						execute(*chunk.blockval,&output);
						result+=output;
						break;
					}
				}
			}
			rescall.arguments.emplace_back(Argument::Chunk(move(result)));
		}
		resline.push_back(rescall);
	}
	return resline;
}

i64 Context::execute(const Pipeline &origline,string *outputcollect){
	Pipeline line=preparePipeline(origline);
	i64 retval=0;
	for(const Call &call : line){
		if(call.arguments.size()==0){
			cerr<<"kaash: Cannot run empty call"<<endl;
			return 1;
		}

		unordered_map<int, int> filedescs = { { 0, 0 }, { 1, 1 }, { 2, 2 } };
		for (const pair<int, Redirect> &pair : call.redirects) {
			int filedesc;

			const Redirect &red = pair.second;
			switch (red.type) {
			case Redirect::Type::fd: {
				auto it = filedescs.find(red.fdval);
				if(it != filedescs.end()) {
					filedesc = it->second;
				} else {
					filedesc = red.fdval;
				}
				break;
			}

			case Redirect::Type::file:
				filedesc = open(red.fileval.data(), red.isoutput ? O_WRONLY : O_RDONLY);
				break;
			}

			filedescs[pair.first] = filedesc;
		}

		const string &cmdname=call.arguments[0].chunks[0].strval;
		Maybe<Block> mblock=varstore.getfunction(cmdname);
		if(mblock.isJust()){
			throw logic_error("UNIMPLEMENTED: Running function variables");
		} else {
			auto it=builtins.find(cmdname);
			if(it!=builtins.end()){
				vector<string> args;

				for (size_t i=1;i<call.arguments.size();i++) {
					args.push_back(call.arguments[i].chunks[0].strval);
				}

				retval=it->second(args, filedescs);
			} else {
				throw logic_error("UNIMPLEMENTED: Running external programs");
			}
		}
	}
	return retval;
}

i64 Context::execute(const Block &block,string *outputcollect){
	i64 retval=0;
	for(const Pipeline &line : block.lines){
		retval=execute(line,outputcollect);
	}
	return retval;
}
