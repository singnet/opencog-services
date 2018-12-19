#include "Ghost.h"
#include <ctime>
#include "../utils.h"

using namespace opencogservices;
using namespace std;

Ghost::Ghost()
{
}

Ghost::~Ghost()
{
}

void Ghost::loadRuleFile(std::string &output, const int token, const std::string &rUrl)
{
	// status to be returned
	string out_msg = "";

	// load url into file
	string atomese_file_name = "";
    string error_msg = "";
	CURLcode error_code = loadUrlFile(error_msg, rUrl, atomese_file_name);

	if (error_code) {
        output.assign(error_msg);
	} else {
		// prepare commands to load
		string command = "(ghost-parse-files \"" + atomese_file_name + "\")";

		// send command to this session
        evaluateScheme(output, command, token);
	}

	// remove temp file
    if (remove(atomese_file_name.c_str())) {
        string msg = "loadAtomeseFile(): Error deleting temporary file: ";
        msg += atomese_file_name;
        logger().warn(msg);
    }

    // assign final output
    output.assign(out_msg);
}

void Ghost::ghostStartSession(const string &rUrl, string &rOutput)
{
	int session_token = 0;

    // build modules to be used by this guile session
    vector<string> modules;
    modules.push_back("/opencog/build/opencog/attention/libattention.so");

    // set agents to be used by this guile session
    vector<string> agents;
    agents.push_back("opencog::AFImportanceDiffusionAgent");
    agents.push_back("opencog::WAImportanceDiffusionAgent");
    agents.push_back("opencog::AFRentCollectionAgent");
    agents.push_back("opencog::WARentCollectionAgent");

	// try to start a new session
	createGuileSession(session_token, &modules, &agents);

	// initialize ghost in this session
	string scheme_out = "";
	evaluateScheme(scheme_out, string("(use-modules (opencog))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog nlp) (opencog nlp lg-dict) (opencog nlp relex2logic) (opencog nlp chatbot))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog nlp sureal) (opencog nlp microplanning))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog nlp aiml) (opencog openpsi))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog ghost))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog ghost procedures))"), session_token);
	evaluateScheme(scheme_out, string("(use-modules (opencog cogserver))"), session_token);

    char* relex_container_name = getenv("RELEX_CONTAINER_NAME");
    string relex_seek_cmd = string("(use-relex-server \"") + relex_container_name + string("\" 4444)");

	evaluateScheme(scheme_out, relex_seek_cmd, session_token);
	evaluateScheme(scheme_out, string("(ecan-based-ghost-rules #t)"), session_token);

	// load url rule file
	loadRuleFile(rOutput, session_token, rUrl);

	evaluateScheme(scheme_out, string("(ghost-run)"), session_token);

	// set output to the user
	rOutput = "session started: " + to_string(session_token);
}

void Ghost::getGhostResponse(const int token, std::string &rOutput, double wait_for_response_secs)
{
	string output = "";

	double elapsed_secs = 0.0;

	while(elapsed_secs < wait_for_response_secs) {
		auto start = chrono::steady_clock::now();

  		evaluateScheme(output, string("(map cog-name (ghost-get-result))"), token);

		if (output.length() > 3 && output != "NOTHING") {
			rOutput.assign(output);
			return;
		}

		auto end = chrono::steady_clock::now();
		auto elapsed = end - start;

  		elapsed_secs += chrono::duration <double, milli> (elapsed).count() / 1000.0;
	}

    rOutput.assign("I have nothing to say...");
}

void Ghost::utterance(const int token, const string &rUtterance, string &rOutput)
{
	// preparing ghost query cmd
	string cmd = "(ghost \"" + rUtterance + "\")";

	// send query to ghost
	evaluateScheme(rOutput, cmd, token);

	// read ghost response
	double wait_for_response_secs = 5.0;
	getGhostResponse(token, rOutput, wait_for_response_secs);
}

void Ghost::ghostEndSession(const int token, string &rOutput)
{
	closeGuileSession(token);
	rOutput = string(GHOST_MSG_SESSION_ENDED);
}

int Ghost::getCommand(const string &rCmdStr)
{
	int command = NOTHING;

	if (rCmdStr == S_TALK)
		return TALK;
	if (rCmdStr == S_START_SESSION)
		return START_SESSION;
	if (rCmdStr == S_END_SESSION)
		return END_SESSION;

	return command;
}

bool Ghost::execute(string &rOutput, const vector<string> &rArgs)
{
	// bot response
	string response = "";

	// Send startup message after receiving an empty argument list
	if (rArgs.size() == 0) {
		rOutput.assign("Usage: use 'Ghost start_session <url>' to start a new session\n \
            Ghost end_session <id> to end a session\n \
            Ghost utterance <utterance string> to talk with ghost.");

		return GHOST_STATUS_OK;
	}

	// try to parse command if any is received
	int command = getCommand(rArgs[0]);

	switch (command) {
		case TALK:
			if (rArgs.size() < TALK_ARG_SIZE) {
				response = "Usage: Ghost utterance <session_id> \"Utterance string\"";
			} else {
				utterance(atoi(rArgs[1].c_str()), rArgs[2], response);
			}
			break;
		case START_SESSION:
			if (rArgs.size() < START_ARG_SIZE) {
				response = "Usage: Ghost start_session <rule file URL>";
			} else {
				ghostStartSession(rArgs[1], response);
			}
			break;
		case END_SESSION:
			if (rArgs.size() < END_ARG_SIZE) {
				response = "Usage: Ghost end_session <session_id>";
			} else {
				ghostEndSession(atoi(rArgs[1].c_str()), response);
			}
			break;
		default:
			response = string(GHOST_MSG_ERROR_INVALID_COMMAND);
			break;
	}

	// set default response if none is obtained from the system
	if (response.length() == 0) {
		response = string(GHOST_MSG_DEFAULT_RESPONSE);
	}

	// set output to the user
	rOutput = response;

	return GHOST_STATUS_OK;
}
