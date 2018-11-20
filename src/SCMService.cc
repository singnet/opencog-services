#include "SCMService.h"

using namespace opencogservices;
using namespace std;

SCMService::SCMService(const string &scmFileName)
{
    this->schemeFileName = scmFileName;
}

SCMService::~SCMService() 
{
}

bool SCMService::execute(string &output, const vector<string> &args)
{
    evaluateScheme("(load \"" + schemeFileName + "\")");
    string cmd = "(execute (list ";
    for (unsigned int i = 0; i < args.size(); i++) {
        cmd += args.at(i);
        if (i != (args.size() - 1)) {
            cmd += " ";
        }
    }
    cmd += "))";
    //printf("cmd = <%s>\n", cmd.c_str());
    evaluateScheme(output, cmd);
    return false;
}

