#include "PatternMiner.h"

using namespace opencogservices;
using namespace std;

PatternMiner::PatternMiner() 
{
    opencog::config().load("./config/opencog_patternminer_nlp.conf");
}

PatternMiner::~PatternMiner() 
{
}

// This method is called by superclass constructor
void PatternMiner::loadModules()
{
    OpencogSNETService::loadModules();
    evaluateScheme("(use-modules (opencog nlp))");
}

bool PatternMiner::execute(string &output, const vector<std::string> &args)
{
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
    if (loadAtomeseFile(errorMessage, url)) {
        output.assign("Error fetching URL: " + url + "\n" + errorMessage);
        return true;
    }

    opencog::PatternMiner pm(atomSpace);
    pm.runPatternMiner(false);
    int g = opencog::config().get_int("Pattern_Max_Gram");
    vector<opencog::HTreeNode*>& topPatterns = pm.getFinalPatternsForGram(g);
    //printf("g = %d size = %lu\n", g, topPatterns.size());
    string patternStr = pm.unifiedPatternToKeyString(topPatterns[0]->pattern);

    output.assign(patternStr);
    return false;
}
