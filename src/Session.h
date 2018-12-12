#ifndef SESSION_H
#define SESSION_H

#include <string>
#include <unistd.h>

#define S_OP_MESSAGE_SENT 100
#define S_OP_MESSAGE_READ 101
#define S_OP_ERROR_CAN_NOT_SEND_MSG -100
#define S_OP_ERROR_CAN_NOT_READ_MSG -101

using namespace std;

namespace opencogservices
{
class Session
{
private:
	string mWritePipeName;
	string mReadPipeName;

	int mWriteFd;
	int mReadFd;
	int mPid;

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