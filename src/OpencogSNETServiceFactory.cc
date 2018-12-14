#include "OpencogSNETServiceFactory.h"
#include "SCMService.h"
#include "cpp-services/Echo.h"
#include "cpp-services/Ghost.h"

using namespace opencogservices;
using namespace std;

OpencogSNETService *OpencogSNETServiceFactory::factory(const string &serviceName) 
{
    if (serviceName == "Echo") {
        return new Echo();
    } if(serviceName == "Ghost") {
        return new Ghost();
    } else {
        string fname = "src/scm-services/" + serviceName + ".scm";
        if (fileExists(fname)) {
            return new SCMService(fname);
        } else {
            return NULL;
        }
    }
}

bool OpencogSNETServiceFactory::fileExists(const std::string& fname) {
    if (FILE *f = fopen(fname.c_str(), "r")) {
        fclose(f);
        return true;
    } else {
        return false;
    }   
}
