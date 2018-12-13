#include "PatternMiner.h"

namespace opencogservices
{
PatternMiner::PatternMiner()
{
}

PatternMiner::~PatternMiner()
{
}

bool PatternMiner::execute(string &output, const vector<std::string> &args)
{
	string scheme_out = "";
	evaluateScheme(scheme_out, "(use-modules (opencog nlp))");

	if ((args.size() < 1) || (args.size() > 2)) {
		output.assign("Usage: PatternMiner <url> [config]\n\t<url> Valid URL.\n\t[config] Optional JSON array with Opencog configuration parameters.");
		return true;
	}

	string url = args.at(0);

	if (url == "") {
		output.assign("Invalid empty URL");
		return true;
	}

	if (args.size() > 1) {
		setConfigurationParameters(args.at(1));
	}

	string errorMessage;
	loadAtomeseFile(errorMessage, url);

	// fetch current atomspace snapshot
	AtomSpace *atomspace = new AtomSpace();

	opencog::PatternMiner pm(*atomspace);
	pm.runPatternMiner(false);
	int g = opencog::config().get_int("Pattern_Max_Gram");
	vector<opencog::HTreeNode *> &topPatterns = pm.getFinalPatternsForGram(g);
	//printf("g = %d size = %lu\n", g, topPatterns.size());
	string patternStr = pm.unifiedPatternToKeyString(topPatterns[0]->pattern);

	output.assign(patternStr);
	return false;
}
} // namespace opencogservices