#include "GuileSessionManager.h"
#include <fstream>

using namespace std;
using namespace opencogservices;

static void callSession(const string &rSessionProgram, const string &rReadPipe, const string &rWritePipe)
{
	string call = rSessionProgram + " " + rReadPipe + " " + rWritePipe;

	//TODO::change to execv
	system(call.c_str());
	//execl(rSessionProgram.c_str(), rReadPipe.c_str(), rWritePipe.c_str());
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
		string cleanpipes_command = "rm " + session.getWritePipeName() + " " + session.getReadPipeName();
		system(cleanpipes_command.c_str());
	}

	// wait for all threads to die (avoid crash)
	waitForSessions();

	// remove opened closed sessions log files
	string rm_pid_file_cmd = "rm " + string(GSM_SESSION_OPENED_SESSIONS_FILE) + " " + string(GSM_SESSION_CLOSED_SESSIONS_FILE);
	system(rm_pid_file_cmd.c_str());
}

int GuileSessionManager::genId()
{
	return _idGen++;
}

int GuileSessionManager::startSession(int &rOutputToken)
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

	// create a new session process and wait son to create its pipes
	thread *pServiceThread;
	pServiceThread = new thread(callSession, first_arg, second_arg_read_pipe, second_arg_write_pipe);

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
	int session_pid = atoi(session.readMsgLock().c_str());

	// write pid to the sessions pid file, just in case the program get killed
	string s_session_pid = to_string(session_pid);
	s_session_pid.push_back('\n');

	// write the newly created pid into pids log file
	_pidFile = open(GSM_SESSION_OPENED_SESSIONS_FILE, O_CREAT | O_APPEND | O_WRONLY);
	write(_pidFile, s_session_pid.c_str(), s_session_pid.length());
	close(_pidFile);

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
	string cleanpipes_command = "rm " + session.getWritePipeName() + " " + session.getReadPipeName();
	system(cleanpipes_command.c_str());

	string session_pid = to_string(session.getPid());

	// remove it from the sessions map
	_sessions.erase(token);

	// stop and remove thread from here (ensure that it will never runs forever)
	_sessionsThreads.erase(token);

	// write pid of this session since it was closed to keep a log of closed sessions
	session_pid.push_back('\n');

	// add the closed session to the pids log file
	_closedSessionsFile = open(GSM_SESSION_CLOSED_SESSIONS_FILE, O_CREAT | O_APPEND | O_WRONLY);
	write(_closedSessionsFile, session_pid.c_str(), session_pid.length());
	close(_closedSessionsFile);

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

int GuileSessionManager::receiveResponse(const int token, string &rOutput, bool lock)
{
	rOutput = "";

	// check if token is valid
	if (_sessions.find(token) == _sessions.end()) {
		return GSM_OP_ERROR_DOES_NOT_EXIST;
	}

	// get session object
	Session session = _sessions[token];

	// get response from session
	if (lock) {
		rOutput = session.readMsgLock();
	} else {
		rOutput = session.readMsg();
	}

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

vector<int> GuileSessionManager::readOpenedSessionsFile()
{
	// return vector
	vector<int> pids;

	// try to open the opened sessions file
	ifstream pid_file(GSM_SESSION_OPENED_SESSIONS_FILE);
	string readed_line = "";

	if (pid_file.is_open()) {
		while (pid_file.good()) {
			getline(pid_file, readed_line);

			// convert line into PID
			int pid = atoi(readed_line.c_str());
			pids.push_back(pid);
		}

		pid_file.close();

        // remove files from fs
        string rm_pid_file_cmd = "rm " + string(GSM_SESSION_OPENED_SESSIONS_FILE);
	    system(rm_pid_file_cmd.c_str());
	}

	// return the readed pids
	return pids;
}

set<int> GuileSessionManager::readClosedSessionsFile()
{
	// return set
	set<int> pids;

	// try to open the opened sessions file
	ifstream pid_file (GSM_SESSION_CLOSED_SESSIONS_FILE);
	string readed_line = "";

	if (pid_file.is_open())
	{
		while (pid_file.good())
		{
			getline(pid_file, readed_line);
			
			// convert line into PID
			int pid = atoi(readed_line.c_str());
			pids.insert(pid);
		}

		pid_file.close();

        // remove files from fs
	    string rm_pid_file_cmd = "rm " + string(GSM_SESSION_CLOSED_SESSIONS_FILE);
	    system(rm_pid_file_cmd.c_str());
	}

	// return the readed pids
	return pids;
}

int GuileSessionManager::clearEnvironment()
{
	int status = GSM_OP_NO_PROCESS_KILLED;

	// get all closed sessions to avoid killing objects that are already dead (private sessions)
	set<int> closed_sessions_pids = readClosedSessionsFile();

	// read all opened sessions pids
	vector<int> opened_sessions_pids = readOpenedSessionsFile();

	string pids_to_kill = "";
	for (unsigned int pid = 0; pid < opened_sessions_pids.size(); pid++) {
		int pid_to_kill = opened_sessions_pids[pid];

		if (closed_sessions_pids.find(pid_to_kill) == closed_sessions_pids.end()) {
			pids_to_kill.append(to_string(pid_to_kill) + " ");
		}
	}

	// kill all processes if readed pids length is greater than 0
	if (pids_to_kill.length() > 0) {
		// tell system to kill all processes
		string rm_pipes_cmd				  = "rm " + string(GSM_SESSION_PIPE_NAME_PREFIX) + "*";
		string kill_sessions_rm_pipes_cmd = "kill -9 " + pids_to_kill + ";" + rm_pipes_cmd + " > /dev/null";

		// tell system to delete all created pipes
		system(kill_sessions_rm_pipes_cmd.c_str());

		// set return status
		status = GSM_OP_PROCESSES_KILLED;
	}

	// return status
	return status;
}