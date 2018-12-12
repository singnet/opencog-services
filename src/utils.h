#ifndef UTILS_H
#define UTILS_H

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

#define UTILS_OP_URL_LOADED 100
#define UTILS_OP_ERROR_CAN_NOT_CREATE_FILE -100
#define UTILS_OP_ERROR_CAN_NOT_LOAD_URL -101

using namespace std;

namespace opencogservices
{

extern int loadUrlFile(const string &rUrl, string &rOutputFileName);
extern void removeTheLoadedFile();
extern size_t writeData(void *pPtr, size_t size, size_t nmemb, void *pStream);

} // namespace opencogservices

#endif