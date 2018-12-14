#include "opencogServicesDef.h"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <nlohmann/json.hpp>

#include <opencog/guile/SchemeEval.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/util/Config.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

using namespace opencog;
using namespace std;

using json = nlohmann::json;

void sendMsg(int fd, const string &rMsg)
{
	string s_msg = rMsg;
	s_msg.push_back(GSM_COMMAND_DELIMITER);
	write(fd, s_msg.c_str(), s_msg.length());
}

string readMsg(int fd)
{
	string received_msg = "";
	char c;
	do {
		int status = read(fd, &c, sizeof(char));

		if (status == 0) {
			return received_msg;
		}

		if (status == -1) {
			exit(-1);
		}

		if (c != GSM_COMMAND_DELIMITER) {
			received_msg.push_back(c);
		}

	} while (c != GSM_COMMAND_DELIMITER);

	return received_msg;
}

string readMsgLock(int fd)
{
	string msg = "";

	do {
		msg = readMsg(fd);
	} while (msg.length() == 0);

	return msg;
}

void session(int argc, char *argv[])
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

	// Init opencog pAtomspace and eval for this session
	AtomSpace *pAtomspace   = new AtomSpace();
	SchemeEval *pSchemeEval = new SchemeEval(pAtomspace);

	bool load_mode = false;

	while (true) {
		// try to read a command, lock if no one is available
		string received_command = "";
		received_command		= readMsgLock(read_fd);
		printf("%d: Received command: %s\n", my_pid, received_command.c_str());

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
				// break loop and exit 0
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

		printf("%d: Response: %s\n", my_pid, output_mgs.c_str());
		sendMsg(write_fd, output_mgs);
	}
}

int main(int argc, char **argv)
{
	session(argc, argv);
	return 0;
}