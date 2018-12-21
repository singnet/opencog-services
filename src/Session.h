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
#include <string>
#include "opencogServicesDef.h"

namespace opencogservices
{
class Session
{
private:
    std::string _writePipeName;
    std::string _readPipeName;

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
    void setReadPipeName(const std::string &rName);
    void setWritePipeName(const std::string &rName);

    int getReadFd();
    int getWriteFd();
    int getPid();

    std::string getReadPipeName();
    std::string getWritePipeName();

    int sendMsg(const std::string &rMsg);
    std::string readMsg();
};
} // namespace opencogservices
#endif