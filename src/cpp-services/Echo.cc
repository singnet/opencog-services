#include "Echo.h"

using namespace opencog_services;
using namespace std;

Echo::Echo() 
{
}

Echo::~Echo() 
{
}

int Echo::execute(string &output, const vector<std::string> &args)
{
    string out;
    for (unsigned int i = 0; i < args.size(); i++) {
        out += args.at(i);
        if (i != (args.size() - 1)) {
            out += " ";
        }
    }
    //out += "\n";
	printf("TEST: %s\n", args[0].c_str());
    output.assign(out);
    return 0;
}

