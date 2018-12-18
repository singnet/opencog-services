#include <grpc/grpc.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#include <grpcpp/security/server_credentials.h>

#include <future>
#include <thread>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <string>

#include "opencog.grpc.pb.h"
#include "OpencogSNETServiceFactory.h"
#include "OpencogSNETService.h"

// seconds
#define ASYNCHRONOUS_API_UPDATE_INTERVAL ((unsigned int) 60)

#define ASYNCHRONOUS_API_OUTPUT_URL "http://54.203.198.53:7000/ServiceAsynchronousOutput/opencog/"
#define ASYNCHRONOUS_API_OUTPUT_DIR "/home/admin/storage/ServiceAsynchronousOutput/opencog/"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;

using namespace opencogservices;
using namespace std;

static unsigned long int nextTicketCount;
static string baseOutputURL;
static string baseOutputDir;
static GuileSessionManager *gpSessionManager;

static void initGuileSessionManager(char *execPath)
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

static void handleSignal(int sig)
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

class ServiceImpl final : public ServiceDefinition::Service
{

private:

public:

    explicit ServiceImpl() {
        nextTicketCount = 1;
        baseOutputURL = ASYNCHRONOUS_API_OUTPUT_URL;
        baseOutputDir = ASYNCHRONOUS_API_OUTPUT_DIR;
        //baseOutputURL = baseOutputDir = "/tmp/";
    }

    // SERVICE_API

    Status execute(ServerContext* context, const Command* input, CommandOutput* output) override {
        exec_service(context, input, output);
        return Status::OK;
    }

    Status asynchronousTask(ServerContext* context, const Command* input, Ticket* ticket) override {
        string suffix = input->cmd() + "-" + to_string(::getpid()) + to_string(nextTicketCount);
        string url = baseOutputURL + suffix;
        string fname = baseOutputDir + suffix;
        nextTicketCount++;
        ticket->set_url(url);
        thread t(&ServiceImpl::threadJobManager, this, context, input, fname);
        t.detach();
        sleep(1);
        return Status::OK;
    }

	void exec_service(ServerContext* context, const Command* input, CommandOutput* output) {
		OpencogSNETService *opencogService = OpencogSNETServiceFactory::factory(input->cmd());
		if (opencogService == NULL) {
			output->set_s(input->cmd() + ": Opencog service not found");
		} else {
			// set process based guile session manager for this service
			opencogService->setGuileSessionManager(gpSessionManager);

			vector<string> args;
			string out;
			// The 'while (true)' is just to avoid an annoying chain of if's 
			while (true) {
				// feed 'args' with arguments until the first empty string
				// is found
				if (input->arg1() == "") break;
				args.push_back(input->arg1());
				if (input->arg2() == "") break;
				args.push_back(input->arg2());
				if (input->arg3() == "") break;
				args.push_back(input->arg3());
				if (input->arg4() == "") break;
				args.push_back(input->arg4());
				if (input->arg5() == "") break;
				args.push_back(input->arg5());
				// Breaks anyway after feeding 5 arguments
				break;
			}
			if (opencogService->execute(out, args)) {
				output->set_s("Error in " + input->cmd() + ": " + out);
			} else {
				output->set_s(out);
			}

			// free mem
			delete opencogService;
		}
	}

	void threadJobManager(ServerContext* context, const Command* input, const string &fname)
	{
		CommandOutput output;

		auto future = std::async(std::launch::async, &ServiceImpl::exec_service, this, context, input, &output);

		unsigned int count = ASYNCHRONOUS_API_UPDATE_INTERVAL;
		while (true) {
			auto threadStatus = future.wait_for(std::chrono::seconds(0));
			if (threadStatus == std::future_status::ready) {
				break;
			} else {
				if (count == ASYNCHRONOUS_API_UPDATE_INTERVAL) {
					count = 0;
					auto now = std::chrono::system_clock::now();
					auto now_c = std::chrono::system_clock::to_time_t(now);
					//string s = "Work in progress. Last update: " + std::time_put(std::localtime(&now_c), "%Y-%m-%d %X") + "\n";
					char buffer[256];
					strftime(buffer, sizeof(buffer)," Work in progress. Last update: %d-%m-%Y %H:%M:%S\n", std::localtime(&now_c));
					// TODO : this need to be improved to use a more robust method
					FILE *f = fopen(fname.c_str(), "w");
					fputs(buffer, f);
					fclose(f);
				}
				count++;
				sleep(1); // 1 second
			}
		}

		string s = "Service finished. Output:\n" + output.s();
		FILE *f = fopen(fname.c_str(), "w");
		fputs(s.c_str(), f);
		fclose(f);
	}
};

static void RunServer() {
    std::string server_address("0.0.0.0:7032");
    ServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    initGuileSessionManager(argv[0]);
    signal(SIGINT, handleSignal);
    RunServer();
    return 0;
}
