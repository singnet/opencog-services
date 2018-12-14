#ifndef _OPENCOGSERVICES_GUILE_SESSION_MANAGER_H
#define _OPENCOGSERVICES_GUILE_SESSION_MANAGER_H

#include "Session.h"
#include <fcntl.h>
#include <map>
#include <set>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace opencogservices
{
extern void callSession(const string &rSessionProgram, const string &rReadPipe, const string &rWritePipe);

class GuileSessionManager
{
private:
	// sessions ids and objects
	map<int, Session> _sessions;
	map<int, thread *> _sessionsThreads;

	// ids holder
	int _idGen;

	// private to avoid the final user to generate IDs freely
	int genId();

	// oppened sessions pid file
	int _pidFile;

	// closed private sessions file
	int _closedSessionsFile;

	// session executable path
	string _sessionAbsoluteExePath;

	// create file with pids to manage oppened processes
	int clearEnvironment();

	// read openedSessions and returns a vector of pids
	vector<int> readOpenedSessionsFile();

	// read the closed sessions file to avoid trying to kill something that is already dead
	set<int> readClosedSessionsFile();

public:
	GuileSessionManager(const char *guileSessionExePath);
	~GuileSessionManager();

	int startSession(int &rOutputToken);
	int endSession(const int token);
	int sendCommand(const int token, const char *pCmd);
	int sync(const int token);
	int receive(const int token, string &rOutput, bool lock = false);

	void waitForSessions();
};
} // namespace opencogservices

#endif