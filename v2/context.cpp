#include <cstdlib>
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
		const string &cmdname=call.arguments[0].chunks[0].strval;
		Maybe<Block> mblock=varstore.getfunction(cmdname);
		if(mblock.isJust()){
			throw logic_error("UNIMPLEMENTED: Running function variables");
		} else {
			auto it=builtins.find(cmdname);
			if(it!=builtins.end()){
				#error TODO
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
