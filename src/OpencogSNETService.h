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

#include <vector>
#include <string>

#include <opencog/guile/SchemeEval.h>
#include <opencog/atomspace/AtomSpace.h>

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

    /*
     * Loads required SCM modules. Default implementation loads only (use-modules (opencog))
     * This method is called in OpencogSNETService constructor.
     */
    virtual void loadModules();

    // ************************************************************
    // Helper methods and vars

    /*
     * Fetches the contents of the passed URL (which is supposed to be a .scm file
     * with an Atomese knowledge base) and load it in 'atomSpace' (a public
     * AtomSpace field).
     *
     * Returns 'true' if an error occured or 'false' otherwise. When 'true' is
     * returned, an error message is written in 'errorMessage'.
     */
    bool loadAtomeseFile(std::string &errorMessage, const std::string &url);

    /*
     * Uses `schemeEval` to evaluate the passed Scheme string.
     */
    void evaluateScheme(const std::string &scmLine);

    /*
     * Uses `schemeEval` to evaluate the passed Scheme string and return the
     * output of the execution.
     */
    void evaluateScheme(std::string &output, const std::string &scmLine);

    void setConfigurationParameters(const std::string jsonString);

    opencog::AtomSpace atomSpace;

private:

    opencog::SchemeEval *schemeEval;
};

}

#endif // _OPENCOGSERVICES_OPENCOGSNETSERVICE_H
