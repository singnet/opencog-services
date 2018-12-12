#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <opencog/util/Config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "OpencogSNETService.h"
#include "utils.h"

using namespace opencogservices;
using namespace opencog;
using namespace std;
using json = nlohmann::json;

OpencogSNETService::OpencogSNETService()
{
	_sessionToken = 0;
}

OpencogSNETService::~OpencogSNETService()
{
	// stop and delete this service private session
	_sessionManager->endSession(_sessionToken);
}

void OpencogSNETService::setConfigurationParameters(const std::string jsonString)
{
	// TODO::send load command to guile session
    /*auto jasonHash = json::parse(jsonString);
    for (json::iterator it = jasonHash.begin(); it != jasonHash.end(); ++it) {
       config().set(it.key(), it.value());
    }*/
}

int OpencogSNETService::loadAtomeseFile(const string &rUrl)
{
	// wait for rule file to be loaded
	sync(_sessionToken);

	// status to be returned
	int status = OSS_OP_URL_LOADED;

	// load url into file
	string atomese_file_name = "";
	int url_status = loadUrlFile(rUrl, atomese_file_name);

	if (url_status == UTILS_OP_URL_LOADED) {
		// prepare commands to load
		string command = "(primitive-load \"" + atomese_file_name + "\")";

		// send command to this session
		int ssm_status = _sessionManager->sendCommand(_sessionToken, command.c_str());

		// check if command was sent properly to the specified session
		if (ssm_status != SSM_OP_SESSION_COMMAND_SENT) {
			status = OSS_ERROR_CAN_NOT_LOAD_INVALID_SESSION;
		}
	} else {
		status = OSS_ERROR_CAN_NOT_LOAD_URL;
	}

	// wait for rule file to be loaded
	sync(_sessionToken);

	// remove temp file
	if (remove(atomese_file_name.c_str())) {
		status = OSS_ERROR_CAN_NOT_DELETE_TEMP_FILE;
	}

	// return op status
	return status;
}

void OpencogSNETService::evaluateScheme(const string &rScmLine)
{
	// wait for rule file to be loaded
	sync(_sessionToken);

	// sending cmd to this object private session
	_sessionManager->sendCommand(_sessionToken, rScmLine.c_str());

	// get response to prevent buffer stacking
	string out = "";
	_sessionManager->receive(_sessionToken, out, true);
}

void OpencogSNETService::evaluateScheme(string &rOutput, const string &rScmLine)
{
	// wait for rule file to be loaded
	sync(_sessionToken);

	// sending cmd to this object private session
	_sessionManager->sendCommand(_sessionToken, rScmLine.c_str());

	// get response to prevent buffer stacking and set as a return parameter
	_sessionManager->receive(_sessionToken, rOutput, true);
}

int OpencogSNETService::startSession(int &rOutputSessionToken)
{
	return _sessionManager->startSession(rOutputSessionToken);
}

int OpencogSNETService::endSession(const int sessionToken)
{
	return _sessionManager->endSession(sessionToken);
}

int OpencogSNETService::sendCommand(const int token, const char *pCmd)
{
	return _sessionManager->sendCommand(token, pCmd);
}

int OpencogSNETService::sync(const int token)
{
	return _sessionManager->sync(token);
}

int OpencogSNETService::receive(const int token, string &rOutput, bool lock)
{
	return _sessionManager->receive(token, rOutput, lock);
}

//TODO::CHANGE LOGIC TO SEND OBJECT DIRECTLY THROUGH PIPE
int OpencogSNETService::getAtomspace(AtomSpace &rAtomSpace)
{
	// ensure that this command will be sent correctly
	_sessionManager->sync(_sessionToken);

	// tell to the _sessionManager to print the current private atomspace
	_sessionManager->sendCommand(_sessionToken, "(cog-prt-atomspace)");

	// receive the atomspace and lock until obtaining a response
	string atomspace_string = "";
	_sessionManager->receive(_sessionToken, atomspace_string, true);

	// create an eval to return this atomspace
	SchemeEval *eval = new SchemeEval(&rAtomSpace);
	eval->eval(atomspace_string);

	// dispose this eval
	eval = nullptr;

	return OSS_OP_ATOMSPACE_RETRIEVED;
}

int OpencogSNETService::getAtomspace(const int token, AtomSpace &rAtomSpace)
{
	// ensure that this command will be sent correctly
	int sync_status = _sessionManager->sync(token);
	int status = OSS_OP_ATOMSPACE_RETRIEVED;

	if (sync_status == SSM_OP_SESSION_FLUSH_SYNC_COMPLETED) {
		// tell to the _sessionManager to print the current private atomspace
		_sessionManager->sendCommand(token, "(cog-prt-atomspace)");

		// receive the atomspace and lock until obtaining a response
		string atomspace_string = "";
		_sessionManager->receive(token, atomspace_string, true);

		// create an eval to return this atomspace
		SchemeEval *eval = new SchemeEval(&rAtomSpace);
		eval->eval(atomspace_string);

		// dispose this eval
		eval = nullptr;
	} else {
		// set error status
		status = OSS_ERROR_ATOMSPACE_CAN_NOT_BE_RETRIVED;
	}

	return status;
}

void OpencogSNETService::setGuileSessionManager(GuileSessionManager *_sessionManager)
{
	// set _sessionManager instancef
	this->_sessionManager = _sessionManager;

	// created a private session for this service
	int status = _sessionManager->startSession(_sessionToken);

	if (status == SSM_OP_SESSION_CREATED) {
		// after sessint this _sessionManager load the opencog modules
		_sessionManager->sendCommand(_sessionToken, "(use-modules (opencog))");

		// lock to sync with session with a OK handshake
		int flush_status = _sessionManager->sync(_sessionToken);

		if (flush_status != SSM_OP_SESSION_FLUSH_SYNC_COMPLETED) {
			exit(OSS_ERROR_FATAL_CAN_NOT_CREATE_PRIVATE_SESSION);
		}
	}
}
