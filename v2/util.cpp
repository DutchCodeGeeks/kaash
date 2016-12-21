#include "util.h"

using namespace std;


string readstream(istream &in){
	const int bufsize=1024;

	string res;
	char buf[bufsize];
	while(in){
		in.read(buf,bufsize);
		int nread=in.gcount();
		if(nread<=0)break;
		res.append(buf,nread);
	}
	return res;
}
