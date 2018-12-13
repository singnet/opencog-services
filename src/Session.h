#ifndef _OPENCOGSERVICES_SESSION_H
#define _OPENCOGSERVICES_SESSION_H

#include "opencogServicesCore.h"

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
	int sendMsg(const char *pMsg);
	string readMsg();
	string readMsgLock();
};
} // namespace opencogservices
#endif