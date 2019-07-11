#include "utils.h"
#include <curl/curl.h>

using namespace opencog_services;

static size_t writeData(void *pPtr, size_t size, size_t nmemb, void *pStream)
{
    size_t written = fwrite(pPtr, size, nmemb, (FILE *)pStream);
    return written;
}

CURLcode opencog_services::loadUrlFile(std::string &errorMessage, const std::string &rUrl, std::string &rOutputFileName)
{
    // create temp file to hold the URL contents
    CURL *pCurlHandle;
    char tmp_name[] = "/tmp/URL_LOADED_FILE_XXXXXX";
    FILE *pFile = fdopen(mkstemp(tmp_name), "w");

    // load URL and save its contents into the file
    curl_global_init(CURL_GLOBAL_ALL);
    pCurlHandle = curl_easy_init();
    curl_easy_setopt(pCurlHandle, CURLOPT_URL, rUrl.c_str());
    curl_easy_setopt(pCurlHandle, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(pCurlHandle, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCurlHandle, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(pCurlHandle, CURLOPT_WRITEDATA, pFile);

    // if unable to handle url set error status, does not override file error
    CURLcode errorCode = curl_easy_perform(pCurlHandle);
    if (errorCode) {
        errorMessage.assign(curl_easy_strerror(errorCode));
    }

    // close the file
    fclose(pFile);
    curl_easy_cleanup(pCurlHandle);
    curl_global_cleanup();

    // set the outputfile name
    rOutputFileName = std::string(tmp_name);

    // return the generated file
    return errorCode;
}