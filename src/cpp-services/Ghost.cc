#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <opencog/util/Config.h>

#include "../utils.h"
#include "Ghost.h"

using namespace opencogservices;
using namespace std;

Ghost::Ghost()
{
	// Init
	mName = "Betty";
}

Ghost::~Ghost()
{
}

int Ghost::loadRuleFile(const int token, const string &rUrl)
{
	// discard all responses from session until now to sync with this service
	flushsync(token);

	// status to be returned
	int status = GHOST_OP_URL_LOADED;

	// load url into file
	string rule_file_name = "";
	int url_status = loadUrlFile(rUrl, rule_file_name);

	if (url_status == UTILS_OP_URL_LOADED) {
		// prepare commands to load file in ghost
		string command = "(ghost-parse-files \"" + rule_file_name + "\")";

		// send command to this session
		int ssm_status = sendCommand(token, command.c_str());

		// check if command was sent properly to the specified session
		if (ssm_status != SSM_OP_SESSION_COMMAND_SENT) {
			status = GHOST_ERROR_CAN_NOT_LOAD_INVALID_SESSION;
		}
	} else {
		status = GHOST_ERROR_CAN_NOT_LOAD_URL;
	}

	// wait for rule file to be loaded
	flushsync(token);

	// remove temp file
	if (remove(rule_file_name.c_str())) {
		status = GHOST_ERROR_CAN_NOT_DELETE_TEMP_FILE;
	}

	// return op status
	return status;
}

void Ghost::ghostStartSession(const string &rUrl, string &rOutput)
{
	int session_token = 0;

	// try to start a new session
	int status = startSession(session_token);

	// set token for the user
	rOutput = to_string(session_token);

	if (status == SSM_OP_SESSION_CREATED) {
		// initialize ghost in this session
		sendCommand(session_token, "(use-modules (opencog))");
		sendCommand(session_token, "(use-modules (opencog nlp) (opencog nlp lg-dict) (opencog nlp relex2logic) (opencog nlp chatbot))");
		sendCommand(session_token, "(use-modules (opencog nlp sureal) (opencog nlp microplanning))");
		sendCommand(session_token, "(use-modules (opencog nlp aiml) (opencog openpsi))");
		sendCommand(session_token, "(use-modules (opencog ghost))");
		sendCommand(session_token, "(use-modules (opencog ghost procedures))");
		sendCommand(session_token, "(use-modules (opencog cogserver))");
		sendCommand(session_token, "(set-relex-server-host)");
		sendCommand(session_token, "(ecan-based-ghost-rules #t)");
		sendCommand(session_token, "(ghost-af-only #f)");

		// load url rule file
		loadRuleFile(session_token, rUrl);

		sendCommand(session_token, "(ghost-run)");

		// set output to the user
		rOutput = "session started: " + rOutput;
	} else {
		rOutput = string(GHOST_MSG_ERROR_CAN_NOT_START_SESSION);
	}
}

string Ghost::getGhostResponse(const int token, int attempts)
{
	string output = "";
	for (int i = 0; i < attempts; i++) {
		// get response
		sendCommand(token, "(map cog-name (ghost-get-result))");

		// get ghost response
		receive(token, output);

		if (output.length() > 3)
			break;
	}

	return output;
}

void Ghost::utterance(const int token, const string &rUtterance, string &rOutput)
{
	// sync here
	flushsync(token);

	// preparing ghost query cmd
	string cmd = "(ghost \"" + rUtterance + "\")";

	// send query to ghost
	int ssm_status = sendCommand(token, cmd.c_str());

	if (ssm_status == SSM_OP_SESSION_COMMAND_SENT) {
		// force scheme to be evaluated
		string output = "";
		receive(token, output, true);

		// read ghost response
		int attempts = 10000;
		rOutput = getGhostResponse(token, attempts);
	} else {
		rOutput = string(GHOST_MSG_ERROR_UNABLE_TO_SEND_UTTERANCE);
	}
}

void Ghost::ghostEndSession(const int token, string &rOutput)
{
	int ssm_status = endSession(token);

	if (ssm_status == SSM_OP_SESSION_ENDED) {
		rOutput = string(GHOST_MSG_SESSION_ENDED);
	} else {
		rOutput = string(GHOST_MSG_SESSION_DOES_NOT_EXIST);
	}
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
		rOutput.assign("I am " + mName);
		return GHOST_STATUS_OK;
	}

	// try to parse command if any is received
	int command = getCommand(rArgs[0]);

	switch (command) {
		case TALK:
			if (rArgs.size() < TALK_ARG_SIZE) {
				response = string(GHOST_MSG_ERROR_NEED_MORE_ARGUMENTS) + rArgs[0] + " command.\n";
			} else {
				utterance(atoi(rArgs[1].c_str()), rArgs[2], response);
			}
			break;
		case START_SESSION:
			if (rArgs.size() < START_ARG_SIZE) {
				response = string(GHOST_MSG_ERROR_NEED_MORE_ARGUMENTS) + rArgs[0] + " command.\n";
			} else {
				ghostStartSession(rArgs[1], response);
			}
			break;
		case END_SESSION:
			if (rArgs.size() < END_ARG_SIZE) {
				response = string(GHOST_MSG_ERROR_NEED_MORE_ARGUMENTS) + rArgs[0] + " command.\n";
			} else {
				ghostEndSession(atoi(rArgs[1].c_str()), response);
			}
			break;
		default:
			response = string(GHOST_MSG_ERROR_INVALID_COMMAND);
			break;
	}

	// set default response if none is obtained from the system
	if (response.length() == 0)
		response = string(GHOST_MSG_DEFAULT_RESPONSE);

	// set output to the user
	rOutput = response;

	return GHOST_STATUS_OK;
}