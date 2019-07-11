#include "GuileSessionManager.h"
#include <fstream>
#include <signal.h>

using namespace std;
using namespace opencog_services;

static void callSession(const string &rSessionProgram, 
                        const string &rReadPipe, 
                        const string &rWritePipe, 
                        const vector<string> *modules, 
                        const vector<string> *agents)
{
    string call = rSessionProgram + " " + rReadPipe + " " + rWritePipe + " ";

    if (modules != nullptr) {
        for (unsigned int module = 0; module < modules->size(); module++) {
            call += (*modules)[module] + " ";
        }
    }

    if (agents != nullptr) {
        for (unsigned int agent = 0; agent < agents->size(); agent++) {
            call += (*agents)[agent] + " ";
        }
    }

    system(call.c_str());
}

GuileSessionManager::GuileSessionManager(const char *guileSessionExePath)
{
    //TODO::change to hex generator
    _idGen = GSM_INITIAL_ID;

    // certify that there will be no running sessions from previously running mannagers
    clearEnvironment();

    // store the path of the guile session exe
    _sessionAbsoluteExePath = string(guileSessionExePath) + "/";
}

GuileSessionManager::~GuileSessionManager()
{
    map<int, Session>::iterator it = _sessions.begin();
    for (; it != _sessions.end(); it++) {
        // get session object
        Session session = it->second;

        // string token
        session.sendMsg(GSM_FINISH_CMD);

        // TODO::check if need to close form this side
        close(session.getWriteFd());
        close(session.getReadFd());

        // TODO::remove created pipes
        remove(session.getWritePipeName().c_str());
        remove(session.getReadPipeName().c_str());   
    }

    // wait for all threads to die (avoid crash)
    waitForSessions();

    // remove opened closed sessions log files
    remove(GSM_SESSION_OPENED_SESSIONS_FILE);
    remove(GSM_SESSION_CLOSED_SESSIONS_FILE);    
}

int GuileSessionManager::genId()
{
    if (_availableIds.size() > 0) {
        int ret = _availableIds.front();
        _availableIds.pop_front();
        return ret;
    }

    return _idGen++;
}

void GuileSessionManager::freeId(int id){
    _availableIds.push_back(id);
}

void GuileSessionManager::buildConfigArgs(std::vector<std::string> *modules, std::vector<std::string> *agents) 
{
    if (modules != nullptr) {
        for (unsigned int module = 0; module < modules->size(); module++) {
            string mod = string(GSM_ARG_TYPE_MODULE) + GSM_COMMAND_DELIMITER + (*modules)[module];
            (*modules)[module].assign(mod);
        }
    }

    if (modules != nullptr) {
        for (unsigned int agent = 0; agent < agents->size(); agent++) {
            string ag = string(GSM_ARG_TYPE_AGENT) + GSM_COMMAND_DELIMITER + (*agents)[agent];
            (*agents)[agent].assign(ag);
        }   
    }
}

int GuileSessionManager::startSession(int &rOutputToken, vector<string> *modules, vector<string> *agents)
{
    // generate an ID for this session
    int id = genId();

    // prepare args, first arg
    string first_arg = _sessionAbsoluteExePath + string(GSM_SESSION_CLIENT);

    // second arg, with pipe name
    string second_arg_read_pipe  = GSM_SESSION_PIPE_NAME_PREFIX + to_string(id) + "_read";
    string second_arg_write_pipe = GSM_SESSION_PIPE_NAME_PREFIX + to_string(id) + "_write";

    // prepara com pipe
    mkfifo(second_arg_read_pipe.c_str(), 0666);
    mkfifo(second_arg_write_pipe.c_str(), 0666);

    // build parameters
    buildConfigArgs(modules, agents);

    // create a new session process and wait son to create its pipes
    thread *pServiceThread;
    pServiceThread = new thread(callSession, first_arg, second_arg_read_pipe, second_arg_write_pipe, modules, agents);

    // open pipes from session _sessionManager, do not use NONBLOCK! otherwise it will lose the handshake-like com.
    int read_fd  = open(second_arg_read_pipe.c_str(), O_RDONLY);
    int write_fd = open(second_arg_write_pipe.c_str(), O_WRONLY);

    if (read_fd < 0 || write_fd < 0) {
        return GSM_OP_ERROR_CAN_NOT_OPEN_PIPES;
    }

    // create a session object to handle messages
    Session session(write_fd, read_fd);

    // set named pipes names just to have a reference for them
    session.setReadPipeName(second_arg_read_pipe);
    session.setWritePipeName(second_arg_write_pipe);

    // wait for the child process to respond with its PID
    int session_pid = atoi(session.readMsg().c_str());

    // write pid to the sessions pid file, just in case the program get killed
    string s_session_pid = to_string(session_pid);

    // write the newly created pid into pids log file
    FILE *pid_file = fopen(GSM_SESSION_OPENED_SESSIONS_FILE, "aw");
    fprintf(pid_file, "%s\n", s_session_pid.c_str());
    fclose(pid_file);

    // set the newly created session PID to allow to kill it when necessary (avoid zombie processess)
    session.setPid(session_pid);

    // add session to the sessions map
    _sessions[id] = session;

    // store the created service thread to do join
    _sessionsThreads[id] = pServiceThread;

    // set output parameter
    rOutputToken = id;

    return GSM_OP_SESSION_CREATED;
}

int GuileSessionManager::endSession(const int token)
{
    // check if session token is positive and if session exists
    if (_sessions.find(token) == _sessions.end()) {
        return GSM_OP_ERROR_DOES_NOT_EXIST;
    }

    // get session object
    Session session = _sessions[token];

    // tell session to close its files and kill itself
    sendCommand(token, GSM_FINISH_CMD);

    // close my side of the pipes
    close(session.getWriteFd());
    close(session.getReadFd());

    // tell the system to delete temp pipes
    remove(session.getWritePipeName().c_str());
    remove(session.getReadPipeName().c_str());

    string session_pid = to_string(session.getPid());

    // remove it from the sessions map
    if (_sessions.find(token) != _sessions.end()) {
        _sessions.erase(token);
    }

    // stop and remove thread from here (ensure that it will never runs forever)
    if (_sessionsThreads.find(token) != _sessionsThreads.end()) {
        // wait for thread to die and then erase it
        _sessionsThreads[token]->join();
        _sessionsThreads.erase(token);
    }

    // add the closed session to the pids log file
    FILE *closed_pid_file = fopen(GSM_SESSION_CLOSED_SESSIONS_FILE, "aw");
    fprintf(closed_pid_file, "%s\n", session_pid.c_str());
    fclose(closed_pid_file);

    // free id
    freeId(token);

    return GSM_OP_SESSION_ENDED;
}

int GuileSessionManager::sendCommand(const int token, const char *pCmd)
{
    // check if token is valid
    if (_sessions.find(token) == _sessions.end()) {
        return GSM_OP_ERROR_UNABLE_TO_SEND_COMMAND;
    }

    // get session object
    Session session = _sessions[token];

    // send cmd to session
    session.sendMsg(pCmd);

    // return status ok
    return GSM_OP_SESSION_COMMAND_SENT;
}

int GuileSessionManager::sync(const int token)
{
    // check if token is valid
    if (_sessions.find(token) == _sessions.end()) {
        return GSM_OP_ERROR_UNABLE_TO_FLUSH_SYNC;
    }

    // get session object
    Session session = _sessions[token];

    // send handshake to sync commands
    session.sendMsg(string(GSM_HANDSHAKE_CMD));

    string cmd = session.readMsg();
    while (cmd != GSM_HANDSHAKE_RESPONSE) {
        cmd = session.readMsg();
    }

    // return sync ok
    return GSM_OP_SESSION_FLUSH_SYNC_COMPLETED;
}

int GuileSessionManager::receiveResponse(const int token, string &rOutput)
{
    rOutput = "";

    // check if token is valid
    if (_sessions.find(token) == _sessions.end()) {
        return GSM_OP_ERROR_DOES_NOT_EXIST;
    }

    // get session object
    Session session = _sessions[token];

    // get response from session
    rOutput = session.readMsg();

    // return op OK
    return GSM_OP_SESSION_MSG_RECEIVED;
}

void GuileSessionManager::waitForSessions()
{
    map<int, thread *>::iterator it = _sessionsThreads.begin();
    for (; it != _sessionsThreads.end(); it++) {
        it->second->join();
    }
}

void GuileSessionManager::readPIDFile(const std::string &filename, set<int> &outPidVec){
    // try to open the opened sessions file
    ifstream pid_file(filename);
    string readed_line = "";

    if (pid_file.is_open()) {
        while (pid_file.good()) {
            getline(pid_file, readed_line);

            // convert line into PID
            int pid = atoi(readed_line.c_str());
            outPidVec.insert(pid);
        }

        pid_file.close();

        // remove files from fs
        remove(filename.c_str());
    }
}

int GuileSessionManager::clearEnvironment()
{
    int status = GSM_OP_NO_PROCESS_KILLED;

    // get all closed sessions to avoid killing objects that are already dead (private sessions)
    std::set<int> closed_sessions_pids;
    readPIDFile(string(GSM_SESSION_CLOSED_SESSIONS_FILE), closed_sessions_pids);

    // read all opened sessions pids
    std::set<int> opened_sessions_pids;
    readPIDFile(string(GSM_SESSION_OPENED_SESSIONS_FILE), opened_sessions_pids);

    string pids_to_kill = "";
    std::set<int>::iterator it = opened_sessions_pids.begin();
    for ( ; it != opened_sessions_pids.end(); it++) {
        int pid_to_kill = *it;

        if (closed_sessions_pids.find(pid_to_kill) == closed_sessions_pids.end()) {
            kill(pid_to_kill, SIGKILL);
        }
    }

    // easiest to remove this way to avoid permission denied due to pipes left openned
    system("rm -rf /tmp/cog* > /dev/null");

    // return status
    return status;
}