#ifndef GHOST_SERVICE_H
#define GHOST_SERVICE_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemeEval.h>
#include <opencog/learning/PatternMiner/PatternMiner.h>
#include <opencog/util/Config.h>

#include "../OpencogSNETService.h"

#define S_TALK "utterance"
#define S_START_SESSION "start_session"
#define S_END_SESSION "end_session"

#define NOTHING 0
#define TALK 200
#define START_SESSION 201
#define END_SESSION 202

#define TALK_ARG_SIZE 3
#define START_ARG_SIZE 2
#define END_ARG_SIZE 2

#define GHOST_STATUS_OK false

#define GHOST_MSG_ERROR_INVALID_COMMAND "Invalid command."
#define GHOST_MSG_ERROR_CAN_NOT_START_SESSION "Can not start session"
#define GHOST_MSG_ERROR_NEED_MORE_ARGUMENTS "Need more arguments for: "
#define GHOST_MSG_ERROR_UNABLE_TO_SEND_UTTERANCE "Unable to send utterance."

#define GHOST_MSG_TOKENGRANT "Your token ID is: "
#define GHOST_MSG_SESSION_ENDED "Session ended."
#define GHOST_MSG_SESSION_DOES_NOT_EXIST "Session does not exist."
#define GHOST_MSG_DEFAULT_RESPONSE "I have nothing to say..."

#define GHOST_OP_URL_LOADED 100

#define GHOST_ERROR_CAN_NOT_LOAD_URL -100
#define GHOST_ERROR_CAN_NOT_LOAD_INVALID_SESSION -101
#define GHOST_ERROR_CAN_NOT_CREATE_FILE -102
#define GHOST_ERROR_CAN_NOT_DELETE_TEMP_FILE -103

using namespace std;

namespace opencogservices
{
class Ghost : public OpencogSNETService
{
public:
	Ghost();
	~Ghost();

	bool execute(string &rOutput, const vector<string> &rArgs) override;

private:
	string getGhostResponse(const int token, int attempts);
	int getCommand(const string &rCmdStr);
	int loadRuleFile(const int token, const string &rUrl);
	void ghostStartSession(const string &rUrl, string &output);
	void ghostEndSession(const int token, string &output);
	void utterance(const int token, const string &rUtterance, string &rOutput);
	string mName;
};
} // namespace opencogservices
#endif