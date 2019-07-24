#include "OpencogSNETService.h"
#include "utils.h"

using namespace opencog_services;
using namespace std;

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
        _sessionManager->receiveResponse(access_token, output);

        // send json string to be loaded
        GSM_status = _sessionManager->sendCommand(_sessionToken, jsonString.c_str());

        if (GSM_status != GSM_OP_SESSION_COMMAND_SENT) {
            // TODO::raise exception
        } else {
            // wait its response
            _sessionManager->receiveResponse(access_token, output);
        }
    }
}

bool OpencogSNETService::loadAtomeseFile(std::string &output, const string &rUrl, const int token)
{
    int access_token = _sessionToken;
    if (token > 0) {
        access_token = token;
    }

    // status to be returned
    string out_msg = "";

    // load url into file
    string atomese_file_name = "";
    string error_msg = "";
    CURLcode error_code = loadUrlFile(error_msg, rUrl, atomese_file_name);

    if (error_code) {
        output.assign(error_msg);
        return true;
    } else {
        // prepare commands to load
        string command = "(primitive-load \"" + atomese_file_name + "\")";

        // send command to this session
        _sessionManager->sync(access_token);
        int GSM_status = _sessionManager->sendCommand(_sessionToken, command.c_str());

        // check if command was sent properly to the specified session
        if (GSM_status != GSM_OP_SESSION_COMMAND_SENT) {
            out_msg.assign("Can not send command...");
        } else {
            _sessionManager->receiveResponse(access_token, out_msg);
        }
    }

    // remove temp file
    if (remove(atomese_file_name.c_str())) {
        string msg = "loadAtomeseFile(): Error deleting temporary file: ";
        msg += atomese_file_name;
        logger().warn(msg);
    }

    // assign final output
    output.assign(out_msg);
    return false;
}

void OpencogSNETService::evaluateScheme(string &rOutput, const string &rScmLine, const int token)
{
    int access_token = _sessionToken;
    if (token >= 0) {
        access_token = token;
    }

    // TODO::check if sync is really necessary here
    // wait for rule file to be loaded
    //_sessionManager->sync(access_token);

    // sending cmd to this object private session
    int status = _sessionManager->sendCommand(access_token, rScmLine.c_str());

    // get response to prevent buffer stacking and set as a return parameter
    _sessionManager->receiveResponse(access_token, rOutput);

    if (status == GSM_OP_ERROR_UNABLE_TO_SEND_COMMAND) {
        rOutput = "Invalid session.";
    }

    // put '\n' at the end
    rOutput.push_back('\n');
}

void OpencogSNETService::createGuileSession(int &rOutputSessionToken, vector<string> *modules, vector<string> *agents, int *pInGhostID)
{
    int status = _sessionManager->startSession(rOutputSessionToken, pInGhostID, modules, agents);

    //TODO::raise exception
}

void OpencogSNETService::closeGuileSession(const int sessionToken, bool free)
{
    int status = _sessionManager->endSession(sessionToken, free);

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
        _sessionManager->receiveResponse(access_token, atomspace_string);

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