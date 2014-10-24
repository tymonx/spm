/*!
 * @copyright Copyright (c) 2014, Tymoteusz Blazejczyk - www.tymonx.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of spm nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "spm_requests.h"
#include "spm_commands.h"

#define WEAK                    SPM_WEAK(spm_default_cmd)

#define SPM_CMD(DNAME, fname)\
    case SPM_##DNAME##_CMD: { handler = spm_##fname##_cmd; } break

SPM_CMD_FUNC(default);
WEAK SPM_CMD_FUNC(ping);
WEAK SPM_CMD_FUNC(get_fw_version);
WEAK SPM_CMD_FUNC(set_byte);
WEAK SPM_CMD_FUNC(get_byte);

spm_handler_t spm_get_request_handler(unsigned int command) {
    spm_handler_t handler;

    switch (command) {
    SPM_CMD(PING,               ping);
    SPM_CMD(GET_FW_VERSION,     get_fw_version);
    SPM_CMD(SET_BYTE,           set_byte);
    SPM_CMD(GET_BYTE,           get_byte);
    default:
        handler = NULL;
        break;
    }

    return handler;
}

SPM_CMD_FUNC(default) {
    (void)data;
    (void)size;

    if (SPM_OK == status) {
        spm_send_error(inst, SPM_INVALID_COMMAND, command);
    }
}
