#ifndef _OPENCOGSERVICES_OPENCOGSERVICESBASE_H
#define _OPENCOGSERVICES_OPENCOGSERVICESBASE_H

#include <chrono>
#include <ctime>
#include <curl/curl.h>
#include <fcntl.h>
#include <future>
#include <iomanip>
#include <list>
#include <map>
#include <nlohmann/json.hpp>
#include <regex>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include <boost/algorithm/string/classification.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemeEval.h>
#include <opencog/learning/PatternMiner/PatternMiner.h>
#include <opencog/util/Config.h>

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>

#include "opencog.grpc.pb.h"

// seconds
#define ASYNCHRONOUS_API_UPDATE_INTERVAL ((unsigned int)60)

#define ASYNCHRONOUS_API_OUTPUT_URL "http://54.203.198.53:7000/ServiceAsynchronousOutput/opencog/"
#define ASYNCHRONOUS_API_OUTPUT_DIR "/home/admin/storage/ServiceAsynchronousOutput/opencog/"

#define UTILS_OP_URL_LOADED 100
#define UTILS_OP_ERROR_CAN_NOT_CREATE_FILE -100
#define UTILS_OP_ERROR_CAN_NOT_LOAD_URL -101

#define GSM_HANDSHAKE_RESPONSE "ok"
#define GSM_COMMAND_DELIMITER '~'

#define GSM_SESSION_PIPE_NAME_PREFIX "/tmp/cog_session_"
#define GSM_SESSION_OPENED_SESSIONS_FILE "/tmp/guile_session_manager_sessions"
#define GSM_SESSION_CLOSED_SESSIONS_FILE "/tmp/guile_session_manager_closed_sessions"
#define GSM_SESSION_CLIENT "guileSession"
#define GSM_HANDSHAKE_CMD "handshake"
#define GSM_LOAD_ATOMESE "load"
#define GSM_CLOSE_DESCS "close"
#define GSM_FINISH_CMD "end"
#define GSM_SET_CONFIG "config"

#define GSM_OP_ERROR_CAN_NOT_OPEN_PIPES -100
#define GSM_OP_ERROR_DOES_NOT_EXIST -101
#define GSM_OP_ERROR_UNABLE_TO_SEND_COMMAND -102
#define GSM_OP_ERROR_UNABLE_TO_FLUSH_SYNC -103

#define GSM_OP_NO_PROCESS_KILLED 200
#define GSM_OP_PROCESSES_KILLED 201

#define GSM_OP_SESSION_CREATED 100
#define GSM_OP_SESSION_ENDED 101
#define GSM_OP_SESSION_COMMAND_SENT 102
#define GSM_OP_SESSION_FLUSH_SYNC_COMPLETED 103
#define GSM_OP_SESSION_MSG_RECEIVED 104

#define GSM_MININUM_RESPONSE_LENGTH 2
#define GSM_INITIAL_ID 2
#define GSM_RESERVED_ID 0
#define GSM_UNDEFINED_ID -1

#define S_OP_MESSAGE_SENT 100
#define S_OP_MESSAGE_READ 101
#define S_OP_ERROR_CAN_NOT_SEND_MSG -100
#define S_OP_ERROR_CAN_NOT_READ_MSG -101

#define OSS_OP_URL_LOADED 100
#define OSS_OP_ATOMSPACE_RETRIEVED 101

#define OSS_ERROR_CAN_NOT_DELETE_TEMP_FILE -100
#define OSS_ERROR_CAN_NOT_LOAD_URL -101
#define OSS_ERROR_CAN_NOT_LOAD_INVALID_SESSION -102
#define OSS_ERROR_ATOMSPACE_CAN_NOT_BE_RETRIVED -104
#define OSS_ERROR_FATAL_CAN_NOT_CREATE_PRIVATE_SESSION -500

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerReaderWriter;
using grpc::ServerWriter;
using grpc::Status;
using namespace opencog;
using namespace std;

static unsigned long int nextTicketCount;
static string baseOutputURL;
static string baseOutputDir;

namespace opencogservices
{
extern void callSession(const string &rSessionProgram, const string &rReadPipe, const string &rWritePipe);
extern int loadUrlFile(const string &rUrl, string &rOutputFileName);
extern size_t writeData(void *pPtr, size_t size, size_t nmemb, void *pStream);
} // namespace opencogservices

#endif