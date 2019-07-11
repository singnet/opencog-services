#include "Session.h"

using namespace opencog_services;
using namespace std;

Session::Session(int write_fd, int read_fd)
{
    this->_pid = -1;
    this->_writeFd = write_fd;
    this->_readFd = read_fd;
}

Session::Session(int pid, int write_fd, int read_fd)
{
    this->_pid = pid;
    this->_writeFd = write_fd;
    this->_readFd = read_fd;
}

Session::Session()
{
    this->_pid = -1;
    this->_writeFd = -1;
    this->_readFd = -1;
}

Session::~Session()
{
}

int Session::sendMsg(const string &msg)
{
    string s_msg = msg;
    s_msg.push_back(GSM_COMMAND_DELIMITER);
    int status = write(_writeFd, s_msg.c_str(), s_msg.length());

    if (status != 0) {
        return S_OP_ERROR_CAN_NOT_SEND_MSG;
    }

    return S_OP_MESSAGE_SENT;
}

string Session::readMsg()
{
    string msg = "";

    do {
        char c;
        do {
            int status = read(_readFd, &c, sizeof(char));

            if (status == 0) {
                return msg;
            }

            if (status == -1) {
                exit(-1);
            }

            if (c != GSM_COMMAND_DELIMITER) {
                msg.push_back(c);
            }

        } while (c != GSM_COMMAND_DELIMITER);
    } while (msg.length() == 0);

    return msg;
}

void Session::setWriteFd(int fd)
{
    this->_writeFd = fd;
}

void Session::setReadFd(int fd)
{
    this->_readFd = fd;
}

void Session::setPid(int pid)
{
    this->_pid = pid;
}

void Session::setReadPipeName(const string &rName)
{
    this->_readPipeName.assign(rName);
}

void Session::setWritePipeName(const string &rName)
{
    this->_writePipeName.assign(rName);
}

int Session::getReadFd()
{
    return _readFd;
}

int Session::getWriteFd()
{
    return _writeFd;
}

int Session::getPid()
{
    return _pid;
}

string Session::getReadPipeName()
{
    return _readPipeName;
}

string Session::getWritePipeName()
{
    return _writePipeName;
}