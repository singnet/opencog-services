/*
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Authors: Alysson Ribeiro da Silva <https://github.com/Ophien>
 * 			Andre Senna <https://github.com/andre-senna>
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

#ifndef _OPENCOGSERVICES_SESSION_H
#define _OPENCOGSERVICES_SESSION_H

#include <unistd.h>
#include "opencogServicesDef.h"

namespace opencogservices
{
class Session
{
private:
	string _writePipeName;
	string _readPipeName;

	int _writeFd;
	int _readFd;
	int _pid;

public:
	Session(int writeFd, int readFd, int pid);
	Session(int writeFd, int readFd);
	Session();
	~Session();

	void setWriteFd(int fd);
	void setReadFd(int fd);
	void setPid(int pid);
	void setReadPipeName(const string &rName);
	void setWritePipeName(const string &rName);
	void setReadPipeName(const char *pName);
	void setWritePipeName(const char *pName);

	int getReadFd();
	int getWriteFd();
	int getPid();

	string getPidStr();
	string getReadPipeName();
	string getWritePipeName();

	int sendMsg(const string &rMsg);
	string readMsg();
	string readMsgLock();
};
} // namespace opencogservices
#endif