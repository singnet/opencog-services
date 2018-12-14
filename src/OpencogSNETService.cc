#include "OpencogSNETService.h"

namespace opencogservices
{

OpencogSNETService::OpencogSNETService()
{
}

OpencogSNETService::~OpencogSNETService()
{
	// delete this service private session
	_sessionManager->endSession(_sessionToken);
}

void OpencogSNETService::setConfigurationParameters(const std::string jsonString, const int token)
{
	int access_token = _sessionToken;
	if (token > 0) {
		access_token = token;
	}

	// sync with manager
	_sessionManager->sync(access_token);

	// send intent to load config
	int GSM_status = _sessionManager->sendCommand(_sessionToken, GSM_SET_CONFIG);

	if (GSM_status != GSM_OP_SESSION_COMMAND_SENT) {
		// TODO::raise exception
	} else {
		// receive intent response
		string output = "";
		_sessionManager->receiveResponse(access_token, output, true);

		// send json string to be loaded
		GSM_status = _sessionManager->sendCommand(_sessionToken, jsonString.c_str());

		if (GSM_status != GSM_OP_SESSION_COMMAND_SENT) {
			// TODO::raise exception
		} else {
			// wait its response
			_sessionManager->receiveResponse(access_token, output, true);
		}
	}
}

void OpencogSNETService::loadAtomeseFile(std::string &output, const string &rUrl, const int token)
{
	int access_token = _sessionToken;
	if (token > 0) {
		access_token = token;
	}

	// status to be returned
	int status = OSS_OP_URL_LOADED;

	// load url into file
	string atomese_file_name = "";
	int url_status			 = loadUrlFile(rUrl, atomese_file_name);

	if (url_status == UTILS_OP_URL_LOADED) {
		// prepare commands to load
		string command = "(primitive-load \"" + atomese_file_name + "\")";

		// send command to this session
		_sessionManager->sync(access_token);
		int GSM_status = _sessionManager->sendCommand(_sessionToken, command.c_str());

		// check if command was sent properly to the specified session
		if (GSM_status != GSM_OP_SESSION_COMMAND_SENT) {
			status = OSS_ERROR_CAN_NOT_LOAD_INVALID_SESSION;
		} else {
			_sessionManager->receiveResponse(access_token, output, true);
		}
	} else {
		status = OSS_ERROR_CAN_NOT_LOAD_URL;
	}

	// remove temp file
	if (remove(atomese_file_name.c_str())) {
		status = OSS_ERROR_CAN_NOT_DELETE_TEMP_FILE;
	}

	// TODO::raise exceptions here based on status variable
}

void OpencogSNETService::evaluateScheme(string &rOutput, const string &rScmLine, const int token)
{
	int access_token = _sessionToken;
	if (token >= 0) {
		access_token = token;
	}

	// TODO::check if sync is really necessary here
	// wait for rule file to be loaded
	_sessionManager->sync(access_token);

	// sending cmd to this object private session
	_sessionManager->sendCommand(access_token, rScmLine.c_str());

	// get response to prevent buffer stacking and set as a return parameter
	_sessionManager->receiveResponse(access_token, rOutput, true);

	// put '\n' at the end
	rOutput.push_back('\n');
}

void OpencogSNETService::createGuileSession(int &rOutputSessionToken)
{
	int status = _sessionManager->startSession(rOutputSessionToken);

	//TODO::raise exception
}

void OpencogSNETService::closeGuileSession(const int sessionToken)
{
	int status = _sessionManager->endSession(sessionToken);

	//TODO::raise exception
}

void OpencogSNETService::fetchAtomspaceSnapshop(AtomSpace &rOutAtomSpace, const int token)
{
	int access_token = _sessionToken;
	if (token > 0) {
		access_token = token;
	}

	// ensure that this command will be sent correctly
	int sync_status = _sessionManager->sync(access_token);

	if (sync_status == GSM_OP_SESSION_FLUSH_SYNC_COMPLETED) {
		// tell to the _sessionManager to print the current private atomspace
		_sessionManager->sendCommand(access_token, "(cog-prt-atomspace)");

		// receive the atomspace and lock until obtaining a response
		string atomspace_string = "";
		_sessionManager->receiveResponse(access_token, atomspace_string, true);

		// create an eval to return this atomspace
		SchemeEval *eval = new SchemeEval(&rOutAtomSpace);
		eval->eval(atomspace_string);

		// dispose this eval
		eval = nullptr;
	} else {
		// TODO::raise exceptions here based on status variable
		// set error status_ atomspace can not be retrieved
	}
}

void OpencogSNETService::setGuileSessionManager(GuileSessionManager *_sessionManager)
{
	// set _sessionManager instancef
	this->_sessionManager = _sessionManager;

	// created a private session for this service
	int status = _sessionManager->startSession(_sessionToken);

	if (status == GSM_OP_SESSION_CREATED) {
		// after sessint this _sessionManager load the opencog modules
		_sessionManager->sendCommand(_sessionToken, "(use-modules (opencog))");

		// lock to sync with session with a OK handshake
		int flush_status = _sessionManager->sync(_sessionToken);

		if (flush_status != GSM_OP_SESSION_FLUSH_SYNC_COMPLETED) {
			// TODO::raise exceptions here based on status variable
			//exit(OSS_ERROR_FATAL_CAN_NOT_CREATE_PRIVATE_SESSION);
		}
	}
}
} // namespace opencogservices