#include "OpencogSNETServiceFactory.h"
#include "SCMService.h"
#include "cpp-services/Echo.h"

using namespace opencogservices;

OpencogSNETService *OpencogSNETServiceFactory::factory(const string &serviceName) 
{
	if (serviceName == "Echo") {
        return new Echo();
    } else {
        string fname = "./src/scm-services/" + serviceName + ".scm";
		printf("name: %s\n", fname.c_str());
        if (fileExists(fname)) {
            return new SCMService(fname);
        } else {
            return NULL;
        }
    }
}

bool OpencogSNETServiceFactory::fileExists(const std::string& fname) {
    ifstream f(fname.c_str());
    return f.good();
}
