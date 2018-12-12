#include "Session.h"
#include "guileSessionManagerDeff.h"

namespace opencogservices
{
Session::Session(int write_fd, int read_fd)
{
	this->mPid = -1;
	this->mWriteFd = write_fd;
	this->mReadFd = read_fd;
}

Session::Session(int pid, int write_fd, int read_fd)
{
	this->mPid = pid;
	this->mWriteFd = write_fd;
	this->mReadFd = read_fd;
}

Session::Session()
{
	this->mPid = -1;
	this->mWriteFd = -1;
	this->mReadFd = -1;
}

Session::~Session()
{
}

int Session::sendMsg(const string &msg)
{
	string s_msg = msg;
	s_msg.push_back(SSM_COMMAND_DELIMITER);
	int status = write(mWriteFd, s_msg.c_str(), s_msg.length());

	if (status != 0) {
		return S_OP_ERROR_CAN_NOT_SEND_MSG;
	}

	return S_OP_MESSAGE_SENT;
}

int Session::sendMsg(const char *msg)
{
	string s_msg = string(msg);
	s_msg.push_back(SSM_COMMAND_DELIMITER);
	int status = write(mWriteFd, s_msg.c_str(), s_msg.length());

	if (status != 0) {
		return S_OP_ERROR_CAN_NOT_SEND_MSG;
	}

	return S_OP_MESSAGE_SENT;
}

string Session::readMsg()
{
	string received_msg = "";
	char c;
	do {
		int status = read(mReadFd, &c, sizeof(char));

		if (status == 0) {
			return received_msg;
		}

		if (status == -1) {
			exit(-1);
		}

		if (c != SSM_COMMAND_DELIMITER) {
			received_msg.push_back(c);
		}

	} while (c != SSM_COMMAND_DELIMITER);

	return received_msg;
}

string Session::readMsgLock()
{
	string msg = "";

	do {
		msg = readMsg();
	} while (msg.length() == 0);

	return msg;
}

void Session::setWriteFd(int fd)
{
	this->mWriteFd = fd;
}

void Session::setReadFd(int fd)
{
	this->mReadFd = fd;
}

void Session::setPid(int pid)
{
	this->mPid = pid;
}

void Session::setReadPipeName(const string &rName)
{
	this->mReadPipeName.assign(rName);
}

void Session::setWritePipeName(const string &rName)
{
	this->mWritePipeName.assign(rName);
}

void Session::setReadPipeName(const char *pName)
{
	this->mReadPipeName = string(pName);
}

void Session::setWritePipeName(const char *pName)
{
	this->mWritePipeName = string(pName);
}

int Session::getReadFd()
{
	return mReadFd;
}

int Session::getWriteFd()
{
	return mWriteFd;
}

int Session::getPid()
{
	return mPid;
}

string Session::getPidStr()
{
	return to_string(mPid);
}

string Session::getReadPipeName()
{
	return mReadPipeName;
}

string Session::getWritePipeName()
{
	return mWritePipeName;
}
} // namespace opencogservices