/*
 * Copyright (C) 2018 OpenCog Foundation
 *
 * Authors: Alysson Ribeiro da Silva <https://github.com/Ophien>
 *			Andre Senna <https://github.com/andre-senna>
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

#ifndef _OPENCOGSERVICES_OPENCOGSERVICESDEF_H
#define _OPENCOGSERVICES_OPENCOGSERVICESDEF_H

// seconds
#define UTILS_OP_URL_LOADED 100
#define UTILS_OP_ERROR_CAN_NOT_CREATE_FILE -100
#define UTILS_OP_ERROR_CAN_NOT_LOAD_URL -101

#define GSM_HANDSHAKE_RESPONSE "ok"
#define GSM_COMMAND_DELIMITER '~'

#define GSM_SESSION_PIPE_NAME_PREFIX "/tmp/cog_session_"
#define GSM_SESSION_OPENED_SESSIONS_FILE "/tmp/guile_session_manager_sessions"
#define GSM_SESSION_CLOSED_SESSIONS_FILE "/tmp/guile_session_manager_closed_sessions"
#define GSM_SESSION_CLIENT "guileSession"
#define GSM_HANDSHAKE_CMD "handshake"
#define GSM_LOAD_ATOMESE "load"
#define GSM_CLOSE_DESCS "close"
#define GSM_FINISH_CMD "end"
#define GSM_SET_CONFIG "config"
#define GSM_LOAD_MODULE "load_module"
#define GSM_ARG_TYPE_MODULE "MODULE"
#define GSM_ARG_TYPE_AGENT "AGENT"

#define GSM_OP_ERROR_CAN_NOT_OPEN_PIPES -100
#define GSM_OP_ERROR_DOES_NOT_EXIST -101
#define GSM_OP_ERROR_UNABLE_TO_SEND_COMMAND -102
#define GSM_OP_ERROR_UNABLE_TO_FLUSH_SYNC -103

#define GSM_OP_NO_PROCESS_KILLED 200
#define GSM_OP_PROCESSES_KILLED 201

#define GSM_OP_SESSION_CREATED 100
#define GSM_OP_SESSION_ENDED 101
#define GSM_OP_SESSION_COMMAND_SENT 102
#define GSM_OP_SESSION_FLUSH_SYNC_COMPLETED 103
#define GSM_OP_SESSION_MSG_RECEIVED 104

#define GSM_MININUM_RESPONSE_LENGTH 2
#define GSM_INITIAL_ID 2
#define GSM_RESERVED_ID 0
#define GSM_UNDEFINED_ID -1

#define S_OP_MESSAGE_SENT 100
#define S_OP_MESSAGE_READ 101
#define S_OP_ERROR_CAN_NOT_SEND_MSG -100
#define S_OP_ERROR_CAN_NOT_READ_MSG -101

#define OSS_OP_URL_LOADED 100
#define OSS_OP_ATOMSPACE_RETRIEVED 101

#define OSS_ERROR_CAN_NOT_DELETE_TEMP_FILE -100
#define OSS_ERROR_CAN_NOT_LOAD_URL -101
#define OSS_ERROR_CAN_NOT_LOAD_INVALID_SESSION -102
#define OSS_ERROR_ATOMSPACE_CAN_NOT_BE_RETRIVED -104
#define OSS_ERROR_FATAL_CAN_NOT_CREATE_PRIVATE_SESSION -500

#endif