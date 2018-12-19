/*
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Author: Alysson Ribeiro da Silva <https://github.com/Ophien>
 *		   Andre Senna <https://github.com/andre-senna>
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

#include "opencogServicesDef.h"
#include <fcntl.h>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <unistd.h>

#include <opencog/cogserver/server/CogServer.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemeEval.h>
#include <opencog/util/Config.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

using namespace opencog;
using namespace std;

using json = nlohmann::json;

static vector<string> modules;
static vector<string> agents;

static void sendMsg(int fd, const string &rMsg)
{
	string s_msg = rMsg;
	s_msg.push_back(GSM_COMMAND_DELIMITER);
	write(fd, s_msg.c_str(), s_msg.length());
}

static string readMsg(int readFd)
{
	string msg = "";

	do {
		char c;
        do {
            int status = read(readFd, &c, sizeof(char));

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

static void callForCogAgentHandler(CogServer* pAgentsHandler) {

    // load the server modules specified in the config files
    for (unsigned int module = 0; module < modules.size(); module++) {
        bool load_status = pAgentsHandler->loadModule(modules[module].c_str());

        if (load_status == false) {
            printf("Unable to load modules...\n");
            exit(-1);
        } else {
            printf("Module %s loaded.\n", modules[module].c_str());
        }
    }

    // load ECAN based ghost agents for debug purposes
    for (unsigned int agent = 0; agent < agents.size(); agent++) {
        pAgentsHandler->createAgent(agents[agent].c_str(), true);
        printf("Agent: %s loaded.\n", agents[agent].c_str());
    }

    // run the server's main loop
    pAgentsHandler->serverLoop();
}

static void readArgs(int argc, char *argv[])
{
    for (int arg = 3; arg < argc; arg++) {
        std::string current_arg = string(argv[arg]);

        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(current_arg);
        while (std::getline(tokenStream, token, GSM_COMMAND_DELIMITER)) {
            tokens.push_back(token);
        }

        std::string arg_type  = tokens[0];
        std::string arg_value = tokens[1];

        if (arg_type == GSM_ARG_TYPE_MODULE) {
            modules.push_back(arg_value);
        } else if (arg_type == GSM_ARG_TYPE_AGENT) {
            agents.push_back(arg_value);
        }
    }
}

static void session(int argc, char *argv[])
{
	// openning piper
	char *pSecondArgWritePipe = argv[1];
	char *pSecondArgReadPipe  = argv[2];

	// open pipes and wait for father to open the same
	int write_fd = open(pSecondArgWritePipe, O_WRONLY);
	int read_fd  = open(pSecondArgReadPipe, O_RDONLY);

	// get my PID and write it to the caller process
	int my_pid = ::getpid();

	// write PID to the caller
	sendMsg(write_fd, to_string(my_pid));

    // Initi this session atomspace
	AtomSpace *pAtomspace   = new AtomSpace();

    // craete a COGSERVER that is able to hold several agents that process information
    CogServer* pAgentsHandler = new CogServer(pAtomspace);

    // get this process instance of the scheme evaluator
	SchemeEval *pSchemeEval = ::SchemeEval::get_evaluator(pAtomspace);

    // run cogserver in a separate thread to avoid process lock
    readArgs(argc, argv);
    thread cog_agent_handler_thread(callForCogAgentHandler, pAgentsHandler);

    // identify if a config file will be loaded for this session scheme evalutlator
	bool load_mode = false;

	while (true) {
		// try to read a command, lock if no one is available
		string received_command = "";
		received_command		= readMsg(read_fd);
		// printf("%d: Received command: %s\n", my_pid, received_command.c_str());

		string output_mgs = "";

        if (load_mode) {
			auto jasonHash = json::parse(received_command);
			for (json::iterator it = jasonHash.begin(); it != jasonHash.end(); ++it) {
				config().set(it.key(), it.value());
			}
			load_mode = false;
		} else {
			if (received_command == string(GSM_HANDSHAKE_CMD)) {
				output_mgs = GSM_HANDSHAKE_RESPONSE;
				// printf("%d: Response: %s\n", myPID, output_mgs.c_str());
			} else if (received_command == string(GSM_SET_CONFIG)) {
				load_mode = true;
			} else if (received_command == string(GSM_FINISH_CMD)) {
				break;
			} else {
				// execute the received command
				output_mgs = pSchemeEval->eval(received_command);

				// remove \n and () from response
				boost::remove_erase_if(output_mgs, boost::is_any_of("\n"));

				// need to set simple output for read lock
				if (output_mgs.length() == 0) {
					// printf("%d: Response: %s\n", my_pid, output_mgs.c_str());
					output_mgs.append("NOTHING");
				}
			}
		}

		//printf("%d: Response: %s\n", my_pid, output_mgs.c_str());
		sendMsg(write_fd, output_mgs);
	}

    // TODO::fix this method to avoid seg. fault
    pAgentsHandler->stop();

     // wait for server to die
    cog_agent_handler_thread.join();
}

int main(int argc, char **argv)
{
	session(argc, argv);
	return 0;
}