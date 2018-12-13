#include "opencogServicesCore.h"

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

	// let the user define which config the system will use
	opencog::config().load("./config/opencog_patternminer_nlp.conf");

	while (true) {
		// try to read a command, lock if no one is available
		string received_command = "";
		received_command		= readMsgLock(read_fd);
		//printf("%d: Received command: %s\n", my_pid, received_command.c_str());

		string output_mg = "";
		if (received_command == string(GSM_HANDSHAKE_CMD)) {
			output_mg = GSM_HANDSHAKE_RESPONSE;

			//printf("%d: Response: %s\n", myPID, output_mg.c_str());
		} else if (received_command == string(GSM_SET_CONFIG)) {

		} else if (received_command == string(GSM_FINISH_CMD)) {
			// break loop and exit 0
			break;
		} else {
			// execute the received command
			output_mg = pSchemeEval->eval(received_command);

			// remove \n and () from response
			boost::remove_erase_if(output_mg, boost::is_any_of("\n"));

			if (output_mg.length() > GSM_MININUM_RESPONSE_LENGTH) {
				//printf("%d: Response: %s\n", myPID, output_mg.c_str());
			}
		}

		sendMsg(write_fd, output_mg);
	}
}

int main(int argc, char **argv)
{
	session(argc, argv);
	return 0;
}