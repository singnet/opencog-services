#ifndef SCHEME_SESSION_MANAGER_H
#define SCHEME_SESSION_MANAGER_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemeEval.h>
#include <opencog/util/Config.h>

#include <chrono>
#include <ctime>
#include <fcntl.h>
#include <future>
#include <iomanip>
#include <list>
#include <map>
#include <set>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "Session.h"
#include "guileSessionManagerDeff.h"

using namespace opencog;
using namespace std;

void callSession(const string &rSessionProgram, const string &rReadPipe, const string &rWritePipe);

namespace opencogservices
{
class GuileSessionManager
{
private:
	// sessions ids and objects
	map<int, Session> mSessions;
	map<int, thread *> mSessionsThreads;

	// ids holder
	int mIdGen;

	// available IDs to be used
	list<int> mAvailableIds;

	// private to avoid the final user to generate IDs freely
	int genId();
	int freeId(int id);

	// oppened sessions pid file
	int mPidFile;

	// closed private sessions file
	int mClosedSessionsFile;

	// create file with pids to manage oppened processes
	int killDeadSessions();

	// read openedSessions and returns a vector of pids
	vector<int> readopenedSessionsFile();

	// read the closed sessions file to avoid trying to kill something that is already dead
	set<int> readClosedSessionsFile();

public:
	GuileSessionManager();
	~GuileSessionManager();

	int startSession(int &rOutputToken);
	int endSession(const int token);
	int sendCommand(const int token, const char *pCmd);
	int flushsync(const int token);
	int receive(const int token, string &rOutput, bool lock = false);

	void waitForSessions();
};
} // namespace opencogservices

#endif