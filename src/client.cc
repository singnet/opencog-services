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

using namespace opencogservices;

class ServiceClient {

    public:

        ServiceClient(std::shared_ptr<Channel> channel) : stub_(ServiceDefinition::NewStub(channel)) {
        }

        // TEST_CODE

        void usage()
        {
            printf("Usage: client sync|async <command> <args>\n(0 to 5 args)\n");
            exit(0);
        }

        void doSomething(int argc, char** argv)
        {

            Command cmd;
            Status status;
            ClientContext context;

            if ((argc < 3) || (argc > 8)) {
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

            unsigned int nargs = argc - 3;
            unsigned int p = 0;
            while (true) {
                if (p++ == nargs) break;
                cmd.set_arg1(argv[p + 2]);
                if (p++ == nargs) break;
                cmd.set_arg2(argv[p + 2]);
                if (p++ == nargs) break;
                cmd.set_arg3(argv[p + 2]);
                if (p++ == nargs) break;
                cmd.set_arg4(argv[p + 2]);
                if (p++ == nargs) break;
                cmd.set_arg5(argv[p + 2]);
            }

            if (sync) {
                CommandOutput output;
                status = stub_->execute(&context, cmd, &output);
                if (status.ok()) {
                    printf("%s\n", output.s().c_str());
                }
            } else {
                Ticket ticket;
                status = stub_->asynchronousTask(&context, cmd, &ticket);
                if (status.ok()) {
                    printf("Task \"%s\" started. See results in: %s\n", cmd.cmd().c_str(), ticket.url().c_str());
                }
            }

            if (! status.ok()) {
                printf("RPC failed - %s\n", status.error_message().c_str());
            }
        }

    private:

        std::unique_ptr<ServiceDefinition::Stub> stub_;
};

int main(int argc, char** argv) {
    ServiceClient guide(grpc::CreateChannel("localhost:7070", grpc::InsecureChannelCredentials()));
    guide.doSomething(argc, argv);
    return 0;
}
