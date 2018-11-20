#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
 
#include <opencog/util/Config.h>

#include "OpencogSNETService.h"

using namespace opencogservices;
using namespace opencog;
using namespace std;
using json = nlohmann::json;
 
static size_t writeData(void *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t written = fwrite(ptr, size, nmemb, (FILE *) stream);
  return written;
}
 
OpencogSNETService::OpencogSNETService() 
{
    schemeEval = new SchemeEval(&atomSpace);
    logger().set_level(Logger::INFO);
    logger().set_print_to_stdout_flag(false);
    loadModules();
}

OpencogSNETService::~OpencogSNETService() 
{
}

bool OpencogSNETService::loadAtomeseFile(string &errorMessage, const std::string &url)
{
    CURL *curlHandle;
    char tmpName[] = "/tmp/OpencogSNETService_loadAtomeseFile_XXXXXX";
    FILE *file = fdopen(mkstemp(tmpName), "w");
    if (! file) {
        return true;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, file);

    CURLcode errorCode = curl_easy_perform(curlHandle);
    if (errorCode) {
        errorMessage.assign(curl_easy_strerror(errorCode));
    }

    fclose(file);
    curl_easy_cleanup(curlHandle);
    curl_global_cleanup();

    string command = "(primitive-load \"";
    command += tmpName;
    command += "\")";
    evaluateScheme(command);

    if (remove(tmpName)) {
        string msg = "loadAtomeseFile(): Error deleting temporary file: ";
        msg += tmpName;
        logger().warn(msg);
    }

    return (errorCode != 0);
}

void OpencogSNETService::evaluateScheme(const string &scmLine)
{
    logger().info("evaluateScheme(): " + scmLine);
    logger().info(schemeEval->eval(scmLine));
}

void OpencogSNETService::evaluateScheme(string &output, const string &scmLine)
{
    logger().info("evaluateScheme(): " + scmLine);
    output.assign(schemeEval->eval(scmLine));
    logger().info(output);
}

void OpencogSNETService::setConfigurationParameters(const std::string jsonString)
{
    auto jasonHash = json::parse(jsonString);
    for (json::iterator it = jasonHash.begin(); it != jasonHash.end(); ++it) {
       config().set(it.key(), it.value());
    }
}


void OpencogSNETService::loadModules()
{
    evaluateScheme("(use-modules (opencog))");
}


