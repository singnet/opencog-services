#include "Ghost.h"
#include <ctime>
#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include "../utils.h"

using namespace opencog_services;
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
    //modules.push_back("/usr/local/lib/opencog/libattention.so");

    // set agents to be used by this guile session
    vector<string> agents;
    //agents.push_back("opencog::AFImportanceDiffusionAgent");
    //agents.push_back("opencog::WAImportanceDiffusionAgent");
    //agents.push_back("opencog::AFRentCollectionAgent");
    //agents.push_back("opencog::WARentCollectionAgent");

    // try to start a new session
    createGuileSession(session_token, &modules, &agents);

    // initialize ghost in this session
    string scheme_out = "";
    evaluateScheme(scheme_out, string("(use-modules (opencog) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog nlp) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog nlp relex2logic) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog openpsi) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog ghost) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog ghost procedures) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog exec) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog logger) )"), session_token);
    evaluateScheme(scheme_out, string("(use-modules (opencog persist-sql) )"), session_token);

    if(const char* relex_container_name = std::getenv("RELEX_CONTAINER_NAME"))
    {
        string relex_seek_cmd = string("(use-relex-server \"") + relex_container_name + string("\" 4444)");
        evaluateScheme(scheme_out, relex_seek_cmd, session_token);
    }
    else{
        printf("Warning: Couldn't read RELEX_CONTAINER_NAME using default 127.0.0.1:4444");
        printf("Using default value as set by: https://git.io/fjuql");
    }

    // Disable ECAN
    evaluateScheme(scheme_out, string("(ghost-set-sti-weight 0)"), session_token);
    evaluateScheme(scheme_out, string("(ghost-af-only #f)"), session_token);
    
    // load url rule file
    loadRuleFile(rOutput, session_token, rUrl);

    evaluateScheme(scheme_out, string("(ghost-run)"), session_token);

    // set output to the user
    rOutput = to_string(session_token);
}

void Ghost::getGhostResponse(const int token, std::string &rOutput, double wait_for_response_secs)
{
    string output = "";

    double elapsed_secs = 0.0;

    while(elapsed_secs < wait_for_response_secs) {
        auto start = chrono::steady_clock::now();

        // Observation: this opencog function is inserting an extra \n to the output
        evaluateScheme(output, string("(map cog-name (ghost-get-result))"), token);

        // remove \n and () from response
        // remove () from response.
        boost::remove_erase_if(output, boost::is_any_of("\n()"));

        if (strcmp(output.c_str(), "NOTHING\n") != 0 && output.length() > 0) {
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
    // As suggested by the problem at: https://github.com/opencog/opencog/issues/3484, we need to send (ghost-halt) to avoid segfaulting when quiting.
    string cmd = "(ghost-halt)";
    evaluateScheme(rOutput, cmd, token);
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

int Ghost::execute(string &rOutput, const vector<string> &rArgs)
{
    // bot response
    string response = "";

    // Send startup message after receiving an empty argument list
    if (rArgs.size() == 0) {
        rOutput.assign("\n\nAvailable commands for this service:\n\n \
        1) Start a new session: \n\n \
            Ghost start_session <url> \n \
                -Param: <url> - url containing a GHOST rules file. \n \
                -Output: Integer - session ID integer representing the oppened session ID. \n\n \
        2) End a session: \n\n \
            Ghost end_session <session_id> \n \
                -Param: <session_id> - integer representing a oppened session ID. \n \
                -Output: String - Session ended message. \n\n \
        3) Talk with a GHOST session: \n\n \
            Ghost utterance <session_id> <utterance_string> \n \
                -Param: <session_id> - integer representing a oppened session ID. \n \
                -Param: <utterance_string> - utterance string between \"\" to send to the specified session ID.\n\n \
                -Output: String - GHOST response string. \n\n"
        );

        return 0;
    }

    // try to parse command if any is received
    int command = getCommand(rArgs[0]);
    int status = 0;

    switch (command) {
        case TALK:
            if (rArgs.size() < TALK_ARG_SIZE) {
                response = "\n\n    Usage:\n \
                Ghost utterance <session_id> <utterance_string> \n \
                    -Param: <session_id> - integer representing a oppened session ID. \n \
                    -Param: <utterance_string> - utterance string between \"\" to send to the specified session ID.\n \
                    -Output: String - GHOST response string. \n\n";
            } else {
                // Check whether id is integer. There is another means to fix this
                try {
                    int session_id = boost::lexical_cast<int>(rArgs[1].c_str());
                    utterance(session_id, rArgs[2], response);
                }
                catch (const boost::bad_lexical_cast& e){
                    // TODO this error doesn't get caught but instead propagates to be bad_alloc error.
                    status = 1;
                }
            }
            break;
        case START_SESSION:
            if (rArgs.size() < START_ARG_SIZE) {
                response = "\n\n    Usage:\n \
                Ghost start_session <url> \n \
                    -Param: <url> - url containing a GHOST rules file. \n \
                    -Output: Integer - session ID integer representing the oppened session ID. \n\n";
            } else {
                ghostStartSession(rArgs[1], response);
            }
            break;
        case END_SESSION:
            if (rArgs.size() < END_ARG_SIZE) {
                response = "\n\n    Usage:\n \
                Ghost end_session <session_id> \n \
                    -Param: <session_id> - integer representing a oppened session ID. \n \
                    -Output: String - Session ended message. \n\n ";
            } else {
                ghostEndSession(atoi(rArgs[1].c_str()), response);
            }
            break;
        default:
            response = string(GHOST_MSG_ERROR_INVALID_COMMAND);
	    status = 1;
            break;
    }

    // set default response if none is obtained from the system
    if (response.length() == 0) {
        response = string(GHOST_MSG_DEFAULT_RESPONSE);
    }

    // set output to the user
    rOutput = response;

    return status;
}
