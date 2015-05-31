#include "config.h"

vector<string> readFile(const char *const fname) {
	ifstream f(fname);
	vector<string> lines;
	if (!!f) {
		/*f.seekg(0,ios_base::end);
		int filelen=f.tellg();
		f.seekg(0,ios_base::beg);
		string buf;
		buf.resize(filelen);
		f.read(buf.data(),filelen);
		f.close();*/
		string line;
		while (f) {
			getline(f, line);
			lines.push_back(move(line));
		}
		return lines;
	} else {
		char *message;
		asprintf(&message, "Can't read program '%s', are you sure it exists?", fname);
		throw message;
	}
}
