#include "ServiceImpl.h"

namespace opencogservices
{

ServiceImpl::ServiceImpl(GuileSessionManager &sessionManager)
{
	nextTicketCount = 1;
	baseOutputURL   = ASYNCHRONOUS_API_OUTPUT_URL;
	baseOutputDir   = ASYNCHRONOUS_API_OUTPUT_DIR;

	// create new manager
	_sessionManager = &sessionManager;
}

Status ServiceImpl::execute(ServerContext *context, const Command *input, CommandOutput *output)
{
	exec_service(context, input, output);
	return Status::OK;
}

Status ServiceImpl::asynchronousTask(ServerContext *context, const Command *input, Ticket *ticket)
{
	string suffix = input->cmd() + "-" + to_string(::getpid()) + to_string(nextTicketCount);
	string url	= baseOutputURL + suffix;
	string fname  = baseOutputDir + suffix;
	nextTicketCount++;
	ticket->set_url(url);
	thread t(&ServiceImpl::threadJobManager, this, context, input, fname);
	t.detach();
	sleep(1);
	return Status::OK;
}

void ServiceImpl::exec_service(ServerContext *context, const Command *input, CommandOutput *output)
{
	OpencogSNETService *opencogService = OpencogSNETServiceFactory::factory(input->cmd());
	if (opencogService == NULL) {
		output->set_s(input->cmd() + ": Opencog service not found");
	} else {
		// set the GuileSessionManager here
		opencogService->setGuileSessionManager(_sessionManager);

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

		// dispose the service class
		delete opencogService;
	}
}

void ServiceImpl::threadJobManager(ServerContext *context, const Command *input, const string &fname)
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
				count	  = 0;
				auto now   = std::chrono::system_clock::now();
				auto now_c = std::chrono::system_clock::to_time_t(now);
				//string s = "Work in progress. Last update: " + std::time_put(std::localtime(&now_c), "%Y-%m-%d %X") + "\n";
				char buffer[256];
				strftime(buffer, sizeof(buffer), " Work in progress. Last update: %d-%m-%Y %H:%M:%S\n", std::localtime(&now_c));
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
	FILE *f  = fopen(fname.c_str(), "w");
	fputs(s.c_str(), f);
	fclose(f);
}
} // namespace opencogservices