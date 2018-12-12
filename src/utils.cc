#include "utils.h"

namespace opencogservices
{
size_t writeData(void *pPtr, size_t size, size_t nmemb, void *pStream)
{
	size_t written = fwrite(pPtr, size, nmemb, (FILE *)pStream);
	return written;
}

int loadUrlFile(const string &rUrl, string &rOutputFileName)
{
	// status to be returned
	int status = UTILS_OP_URL_LOADED;

	// create temp file to hold the URL contents
	CURL *pCurlHandle;
	char tmp_name[] = "/tmp/URL_LOADED_FILE_XXXXXX";
	FILE *pFile = fdopen(mkstemp(tmp_name), "w");

	// return error if failed to create file
	if (!pFile) {
		status = UTILS_OP_ERROR_CAN_NOT_CREATE_FILE;
	}

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
	if (errorCode != CURLE_OK && status != UTILS_OP_ERROR_CAN_NOT_CREATE_FILE) {
		status = UTILS_OP_ERROR_CAN_NOT_LOAD_URL;
	}

	// close the file
	fclose(pFile);
	curl_easy_cleanup(pCurlHandle);
	curl_global_cleanup();

	// set the outputfile name
	rOutputFileName = string(tmp_name);

	// return the generated file
	return status;
}
}