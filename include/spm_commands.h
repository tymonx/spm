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

#ifndef _SPM_COMMANDS_H_
#define _SPM_COMMANDS_H_

#include "spm.h"

/* Ping */
#define SPM_PING_CMD                            0x00
#define SPM_PING_RQST_SIZE                      0
#define SPM_PING_RESP_SIZE                      0

/* Get firmware version */
#define SPM_GET_FW_VERSION_CMD                  0x01
#define SPM_GET_FW_VERSION_RQST_SIZE            0
#define SPM_GET_FW_VERSION_RESP_SIZE            4

struct spm_get_fw_version_response {
    uint8_t major;
    uint8_t minor;
    uint8_t bugfix;
    uint8_t build;
}SPM_PACKED;

/* Set byte in shared memory */
#define SPM_SET_BYTE_CMD                        0x02
#define SPM_SET_BYTE_RQST_SIZE                  2
#define SPM_SET_BYTE_RESP_SIZE                  0

struct spm_set_byte_request {
    uint8_t address;
    uint8_t data;
}SPM_PACKED;

/* Get byte in shared memory */
#define SPM_GET_BYTE_CMD                        0x03
#define SPM_GET_BYTE_RQST_SIZE                  1
#define SPM_GET_BYTE_RESP_SIZE                  1

struct spm_get_byte_request {
    uint8_t address;
}SPM_PACKED;

struct spm_get_byte_response {
    uint8_t data;
}SPM_PACKED;

#endif /* _SPM_COMMANDS_H_ */
