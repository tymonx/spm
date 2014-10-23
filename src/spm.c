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

#include "spm.h"
#include "spm_crc.h"
#include "spm_fsm.h"

#include <string.h>

#define MAX_DATA_SIZE         255

static int spm_send_frame(struct spm *inst, unsigned int control,
        unsigned int command, const unsigned char *data, unsigned int size) {
    if (NULL == inst->write) return SPM_ERROR_SEND;
    if (MAX_DATA_SIZE < size) return SPM_ERROR_SEND;

    SPM_MUTEX_UNLOCK(inst);

    unsigned int frame_length = SPM_FRAME_HEADER_SIZE + size + 1;
    unsigned char buffer[frame_length];

    struct spm_frame *frame = (struct spm_frame *)(buffer);
    frame->sync     = (uint8_t)(SPM_FRAME_SYNC);
    frame->control  = (uint8_t)(control);
    frame->command  = (uint8_t)(command);
    frame->size     = (uint8_t)(size);
    if (NULL == data) {
        memset(frame->data, 0, size);
    } else {
        memcpy(frame->data, data, size);
    }
    frame->data[size] = (uint8_t)spm_crc(buffer, frame_length - 1);

    SPM_MUTEX_LOCK(inst);

    return inst->write(buffer, frame_length);
}

int spm_send_request(struct spm *inst, spm_handler_t handler,
        unsigned int command, const void *data, unsigned int size) {

    int status = SPM_ERROR_BUSY;
    struct spm_request *request = inst->tx;

    SPM_MUTEX_LOCK(inst);

    for (unsigned int i = 0; i < SPM_TX_REQUESTS; i++, request++) {
        if (NULL == request->handler) {
            request->time_ms = 0;
            request->options = 0;
            request->command = (uint8_t)command;
            request->handler = handler;
            status = spm_send_frame(inst, SPM_CTRL_REQUEST,
                    command, data, size);
            if (SPM_SUCCESS != status) {
                request->handler = NULL;
            }
            break;
        }
    }

    SPM_MUTEX_UNLOCK(inst);

    return status;
}

int spm_send_response(struct spm *inst, unsigned int resp_status,
        unsigned int command, const void *data, unsigned int size) {

    int status = SPM_ERROR_BUSY;
    struct spm_request *request = inst->rx;

    SPM_MUTEX_LOCK(inst);

    for (unsigned int i = 0; i < SPM_RX_REQUESTS; i++, request++) {
        if ((NULL != request->handler) && (command == request->command)) {
            status = spm_send_frame(inst, SPM_CTRL_STATUS_MASK & resp_status,
                    command, data, size);
            if (SPM_SUCCESS == status) {
                request->handler = NULL;
            }
            break;
        }
    }

    SPM_MUTEX_UNLOCK(inst);

    return status;
}
