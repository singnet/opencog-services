#ifndef GHOST_SERVICE_H
#define GHOST_SERVICE_H

#include <vector>
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

#define GHOST_MSG_TOKENGRANT "Your token ID is: "
#define GHOST_MSG_SESSION_ENDED "Session ended."
#define GHOST_MSG_SESSION_DOES_NOT_EXIST "Session does not exist."
#define GHOST_MSG_DEFAULT_RESPONSE "I have nothing to say..."

#define GHOST_OP_URL_LOADED 100

#define GHOST_ERROR_CAN_NOT_LOAD_URL -100
#define GHOST_ERROR_CAN_NOT_LOAD_INVALID_SESSION -101
#define GHOST_ERROR_CAN_NOT_CREATE_FILE -102
#define GHOST_ERROR_CAN_NOT_DELETE_TEMP_FILE -103

namespace opencogservices
{
class Ghost : public OpencogSNETService
{
public:
	Ghost();
	~Ghost();

	bool execute(std::string &rOutput, const std::vector<std::string> &rArgs) override;

private:
	void getGhostResponse(const int token, std::string &rOutput, double attempts_time);
	int getCommand(const std::string &rCmdStr);
	void loadRuleFile(std::string &output, const int token, const std::string &rUrl);
	void ghostStartSession(const std::string &rUrl, std::string &output);
	void ghostEndSession(const int token, std::string &output);
	void utterance(const int token, const std::string &rUtterance, std::string &rOutput);
};
} // namespace opencogservices
#endif
