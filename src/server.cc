
#include "ServiceImpl.h"

using namespace opencogservices;
using namespace std;

// session manager need to be global in order to be accessed by the signal handler
static GuileSessionManager *gpSessionManager;

void initGuileSessionManager(char *execPath)
{
	// TODO::check better way to do it
	char path_save[PATH_MAX];
	char abs_exe_path[PATH_MAX];
	char *p;

	// TODO::check better way to do it
	if (!(p = strrchr(execPath, '/'))) {
		getcwd(abs_exe_path, sizeof(abs_exe_path));
	} else {
		*p = '\0';
		getcwd(path_save, sizeof(path_save));
		chdir(execPath);
		getcwd(abs_exe_path, sizeof(abs_exe_path));
		chdir(path_save);
	}

	// initialize guile session manager and set the guile session absolute executable path
	gpSessionManager = new GuileSessionManager(abs_exe_path);
}

void handleSignal(int sig)
{
	bool free_guile_session_manager = false;

	switch (sig) {
		case SIGINT:
			free_guile_session_manager = true;
			break;
		case SIGKILL:
			free_guile_session_manager = true;
			break;
		case SIGTERM:
			free_guile_session_manager = true;
			break;
		case SIGPWR:
			free_guile_session_manager = true;
			break;
	}

	if (free_guile_session_manager) {
		/* guarantee that the created sessions will be deleted for any of the registered signals.
		 * otherwise the session manager will kill them during its next startup
		 */
		delete gpSessionManager;
	}

	// exit the program by returning the received signal
	exit(sig);
}

void RunServer()
{
	std::string server_address("0.0.0.0:7032");
	ServiceImpl service(*gpSessionManager);
	ServerBuilder builder;
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;
	server->Wait();
}

int main(int argc, char **argv)
{
	// init global manager to be handled by the handleSignal function
	initGuileSessionManager(argv[0]);

	// register a signal handler for this process
	signal(SIGINT, handleSignal);

	// runs server
	RunServer();

	// return status OK
	return 0;
}
