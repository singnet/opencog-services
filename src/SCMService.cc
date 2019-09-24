#include "SCMService.h"
#include <regex>

using namespace opencog_services;
using namespace std;

SCMService::SCMService(const string &scmFileName)
{
    this->schemeFileName = scmFileName;
}

SCMService::~SCMService() 
{
}

int SCMService::execute(string &output, const vector<string> &args)
{
    std::regex url_regex (
        R"(^http|https:(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)",
        std::regex::extended
    );
    std::smatch url_match_result;

    string scheme_out = "";
    evaluateScheme(scheme_out, "(load \"" + schemeFileName + "\")");

    bool arg_is_url = false;
    if (std::regex_match(args.at(0), url_match_result, url_regex)) {
        printf("Fetching and loading into Atomspace: %s\n", args.at(0).c_str());
        string errorMessage;
        if(loadAtomeseFile(errorMessage, args.at(0))) {
            output.assign(errorMessage);
            return 1;
        }
        arg_is_url = true;
    }

    unsigned int arg_index = 0;
    if(arg_is_url)
        arg_index = 1;

    string cmd = "(execute (list ";
    for (; arg_index < args.size(); arg_index++) {
        cmd += args.at(arg_index);
        if (arg_index != (args.size() - 1)) {
            cmd += " ";
        }
    }
    cmd += "))";
    printf("Scheme command: <%s>\n", cmd.c_str());
    evaluateScheme(output, cmd);
    return 0;
}