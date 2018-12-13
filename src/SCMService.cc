#include "SCMService.h"

namespace opencogservices
{

SCMService::SCMService(const string &scmFileName)
{
	this->schemeFileName = scmFileName;
}

SCMService::~SCMService()
{
}

bool SCMService::execute(string &output, const vector<string> &args)
{
	std::regex url_regex(
		R"(^http|https:(//([^\/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)",
		std::regex::extended);
	std::smatch url_match_result;

	string scheme_out = "";
	evaluateScheme(scheme_out, "(load \"" + schemeFileName + "\")");

	for (unsigned int i = 0; i < args.size(); i++) {
		if (std::regex_match(args.at(i), url_match_result, url_regex)) {
			fprintf(stderr, "Matched: %s\n", args.at(i).c_str());
			string errorMessage;
			loadAtomeseFile(errorMessage, args.at(i));
		} else {
			string cmd		  = "(display \"" + args.at(i) + "\")";
			string scheme_out = "";
			evaluateScheme(scheme_out, cmd);

			output += scheme_out;
			if (i < args.size() - 1) output += " ";
		}
	}
	return false;
}

} // namespace opencogservices