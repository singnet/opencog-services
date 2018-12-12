/*
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Author: Andre Senna <https://github.com/andre-senna>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOGSERVICES_OPENCOGSNETSERVICE_H
#define _OPENCOGSERVICES_OPENCOGSNETSERVICE_H

#include <string>
#include <vector>

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/guile/SchemeEval.h>

#include "GuileSessionManager.h"

#define OSS_OP_URL_LOADED 100
#define OSS_OP_ATOMSPACE_RETRIEVED 101

#define OSS_ERROR_CAN_NOT_DELETE_TEMP_FILE -100
#define OSS_ERROR_CAN_NOT_LOAD_URL -101
#define OSS_ERROR_CAN_NOT_LOAD_INVALID_SESSION -102
#define OSS_ERROR_ATOMSPACE_CAN_NOT_BE_RETRIVED -104
#define OSS_ERROR_FATAL_CAN_NOT_CREATE_PRIVATE_SESSION -500

using namespace opencog;

namespace opencogservices
{

/**
 * Superclass of every Opencog service.
 */
class OpencogSNETService
{
public:
	OpencogSNETService();
	~OpencogSNETService();

    // ************************************************************
    // Required API for subclasses

    /*
     * This is the method called by the gRPC server. It is suppused to do
     * (synchronously) everything the service does. Return 'true' if some error
     * occurred or 'false' otherwise.
     */
    virtual bool execute(std::string &output, const std::vector<std::string> &args) = 0;


    // ************************************************************
    // Helper methods and vars

    /*
     * Fetches the contents of the passed URL (which is supposed to be a .scm file
     * with an Atomese knowledge base) and load it in 'atomSpace' (a public
     * AtomSpace field). Any errors are reported in errorMessage.
     *
     * Returns 'true' if an error occured or 'false' otherwise. When 'true' is
     * returned, an error message is written in 'errorMessage'.
     */
    int loadAtomeseFile(const std::string &url);

    /*
     * Uses `schemeEval` to evaluate the passed Scheme string.
     */
    void evaluateScheme(const std::string &scmLine);

    /*
     * Uses `schemeEval` to evaluate the passed Scheme string and return the
     * output of the execution.
     */
    void evaluateScheme(std::string &output, const std::string &scmLine);

    /*
     * Use the passed JSON hash to set Opencog's configuration parameters (e.g.
     * `{"Max_thread_num": "8", "Pattern_Max_Gram": "3"}`)
     */
    void setConfigurationParameters(const std::string jsonString);

	/*
     * Set sessionmanager instance for 'this' object
     */
	void setGuileSessionManager(GuileSessionManager *pManager);

	/*
     * Session _sessionManager mirrored functions
     */
	int startSession(int &rOutputSessionToken);
	int endSession(const int sessionToken);
	int sendCommand(const int token, const char *pCmd);
	int sync(const int token);
	int receive(const int token, string &rOutput, bool lock = false);
	int getAtomspace(AtomSpace &rOutAtomSpace);
	int getAtomspace(const int token, AtomSpace &rOutAtomSpace);

private:
	// used to have access to sessions
	GuileSessionManager *_sessionManager;

	// used to store a created private session for this service
	int _sessionToken;
};

} // namespace opencogservices

#endif // _OPENCOGSERVICES_OPENCOGSNETSERVICE_H
