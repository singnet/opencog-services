#ifndef _OPENCOGSERVICES_UTIL_H
#define _OPENCOGSERVICES_UTIL_H

#include <string>
#include <curl/curl.h>

namespace opencogservices{
    CURLcode loadUrlFile(std::string &errorMessage, const std::string &rUrl, std::string &rOutputFileName);
}

#endif