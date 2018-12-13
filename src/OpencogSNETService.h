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

#include "opencogServicesCore.h"
#include "GuileSessionManager.h"

namespace opencogservices
{

// declare here and let the compiler figure out which class is this one,
// need to set a friend ref in the SNETService class
class ServiceImpl;

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
	void loadAtomeseFile(std::string &output, const std::string &url, const int token = -1);

	/*
     * Uses `schemeEval` to evaluate the passed Scheme string and return the
     * output of the execution.
     */
	void evaluateScheme(std::string &output, const std::string &scmLine, const int token = -1);

	/*
     * Use the passed JSON hash to set Opencog's configuration parameters (e.g.
     * `{"Max_thread_num": "8", "Pattern_Max_Gram": "3"}`)
     */
	void setConfigurationParameters(const std::string jsonString, const int token = -1);

	/*
     * Session _sessionManager mirrored functions
     */
	void createGuileSession(int &rOutputSessionToken);
	void closeGuileSession(const int sessionToken);
	void fetchAtomspaceSnapshop(AtomSpace &rOutAtomSpace, const int token = -1);

private:
	// let the service class to have access to the setGuileSessionManager
	friend class ServiceImpl;

	// Set sessionmanager instance for 'this' object
	void setGuileSessionManager(GuileSessionManager *pManager);

	// used to have access to sessions
	GuileSessionManager *_sessionManager;

	// used to store a created private session for this service
	int _sessionToken;
};

} // namespace opencogservices

#endif // _OPENCOGSERVICES_OPENCOGSNETSERVICE_H
