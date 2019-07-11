#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include "opencog.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using namespace opencog_services;

class OpencogServicesClient {
    public:
        OpencogServicesClient(std::shared_ptr<Channel> channel) : stub_(OpencogServices::NewStub(channel)) {

        }

        void usage()
        {
            printf("Usage: client sync|async <command> <args>\n(0 to 5 args)\n");
            exit(0);
        }

        void exec(int argc, char** argv)
        {
            Command cmd;
            Status status;
            ClientContext context;

            if (argc < 3) {
                usage();
            } 

            bool sync;
            if (!strcmp(argv[1], "sync")) {
                sync = true;
            } else if (!strcmp(argv[1], "async")) {
                sync = false;
            } else {
                usage();
            }

            cmd.set_cmd(argv[2]);
            
            for (int arg = 3; arg < argc; arg++) {
                cmd.add_params(argv[arg]);
            }

            if (sync) {
                CommandOutput output;
                status = stub_->Execute(&context, cmd, &output);
                if (status.ok()) {
                    printf("%s\n", output.output().c_str());
                }
                else {
                    printf("Error has been reached: %s\n", output.output().c_str());
                }
            } else {
                CommandOutput output;
                status = stub_->AsynchronousTask(&context, cmd, &output);
                if (status.ok()) {
                    //TODO::print results
                }
            }

            if (! status.ok()) {
                printf("RPC failed - %s\n", status.error_message().c_str());
            }
        }

    private:
        std::unique_ptr<OpencogServices::Stub> stub_;
};

int main(int argc, char** argv) {
    std::string server_address = "";
	if (char * server_port = getenv("OPENCOG_SERVER_PORT")) {
		std::string port(server_port);
	    server_address = "0.0.0.0:" + port ;
	}
	else {
		printf("Warning: Using default OPENCOG_SERVER_PORT: 7032\n");
	    server_address = "0.0.0.0:7032";
	}

    OpencogServicesClient opencog_services_client(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()));
    opencog_services_client.exec(argc, argv);
    return 0;
}
