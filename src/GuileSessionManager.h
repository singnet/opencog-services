/*
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Authors: Alysson Ribeiro da Silva <https://github.com/Ophien>
 *			Andre Senna <https://github.com/andre-senna>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOGSERVICES_GUILE_SESSION_MANAGER_H
#define _OPENCOGSERVICES_GUILE_SESSION_MANAGER_H

#include "Session.h"
#include <fcntl.h>
#include <map>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <iterator>

namespace opencogservices
{
class GuileSessionManager
{
private:
	// sessions ids and objects
	std::map<int, Session> _sessions;
	std::map<int, std::thread *> _sessionsThreads;

	// ids holder
	int _idGen;

	// private to avoid the final user to generate IDs freely
	int genId();
    void freeId(int id);
    std::list<int> _availableIds;

	// session executable path
	std::string _sessionAbsoluteExePath;

	// create file with pids to manage oppened processes
	int clearEnvironment();

    // read pid files to check if there is an openned process
    void readPIDFile(const std::string &filename, std::set<int> &outPidVec);

    // configure parameters
    void buildConfigArgs(std::vector<std::string> *modules, std::vector<std::string> *agents);

public:
	GuileSessionManager(const char *guileSessionExePath);
	~GuileSessionManager();

	int startSession(int &rOutputToken, std::vector<std::string> *modules = nullptr, std::vector<std::string> *agents = nullptr);
	int endSession(const int token);
	int sendCommand(const int token, const char *pCmd);
	int sync(const int token);
	int receiveResponse(const int token, std::string &rOutput);

	void waitForSessions();
};
} // namespace opencogservices

#endif