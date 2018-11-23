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

#ifndef _OPENCOGSERVICES_PATTERNMINER_H
#define _OPENCOGSERVICES_PATTERNMINER_H

#include <opencog/util/Config.h>
#include <opencog/atomspace/AtomSpace.h>
#include <opencog/learning/PatternMiner/PatternMiner.h>
#include <opencog/guile/SchemeEval.h>

#include "../OpencogSNETService.h"

namespace opencogservices
{

/**
 *
 */
class PatternMiner : public OpencogSNETService
{

public:

    // OpencogSNETService API
    bool execute(std::string &output, const std::vector<std::string> &args);
    void loadModules();

    PatternMiner();
    ~PatternMiner();

private:

};

}

#endif // _OPENCOGSERVICES_PATTERNMINER_H
