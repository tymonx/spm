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

#include "spm_fsm.h"
#include "spm_crc.h"
#include "spm_buffer.h"
#include "spm_requests.h"

#if SPM_BUFFER_SIZE < 5
#error "Buffer size is too small, it must have enough space for minimal frame"
#endif

static void spm_substate_request(struct spm *inst);
static void spm_substate_response(struct spm *inst);

void spm_state_sync(struct spm *inst, unsigned int sync) {
    if (SPM_FRAME_SYNC == sync) {
        spm_buffer_reset(inst);
        spm_buffer_write(inst, sync);
        spm_state_next(inst, spm_state_control);
    }
}

void spm_state_control(struct spm *inst, unsigned int control) {
    spm_buffer_write(inst, control);
    spm_state_next(inst, spm_state_command);
}

void spm_state_command(struct spm *inst, unsigned int command) {
    spm_buffer_write(inst, command);
    spm_state_next(inst, spm_state_size);
}

void spm_state_size(struct spm *inst, unsigned int size) {
    spm_buffer_write(inst, size);
    if (0 == size) {
        spm_state_next(inst, spm_state_crc);
    } else {
        inst->data_count = (uint16_t)size;
        spm_state_next(inst, spm_state_data);
    }
}

void spm_state_data(struct spm *inst, unsigned int data) {
    if (SPM_OK == spm_buffer_write(inst, data)) {
        inst->data_count--;
        if (0 == inst->data_count) {
            spm_state_next(inst, spm_state_crc);
        }
    } else {
        spm_buffer_shift_and_redispatch(inst, data);
    }
}

void spm_state_crc(struct spm *inst, unsigned int crc) {
    if (SPM_OK == spm_buffer_write(inst, crc)) {
        struct spm_frame *frame = spm_buffer_get_frame(inst);
        if (spm_crc(inst->buffer.data,
                    SPM_FRAME_HEADER_SIZE + frame->size) == crc) {
            if (SPM_CTRL_REQUEST & frame->control) {
                spm_substate_request(inst);
            } else {
                spm_substate_response(inst);
            }
            spm_state_next(inst, spm_state_sync);
        } else {
            if (SPM_CTRL_REQUEST & frame->control) {
                spm_send_error(inst, SPM_INVALID_CRC, frame->command);
            }
            spm_buffer_shift_and_redispatch(inst, SPM_BUFFER_NO_DATA);
        }
    } else {
        spm_buffer_shift_and_redispatch(inst, crc);
    }
}

static void spm_substate_request(struct spm *inst) {
    struct spm_frame *frame = spm_buffer_get_frame(inst);
    struct spm_request *request = inst->rx;

    SPM_MUTEX_LOCK(inst);

    for (unsigned int i = 0; i < SPM_RX_REQUESTS; i++, request++) {
        if (NULL == request->handler) {
            request->time_ms = 0;
            request->options = SPM_OPTIONS_RX;
            request->command = frame->command;
            request->handler = spm_get_request_handler(frame->command);
            spm_handler_t request_handler = request->handler;

            SPM_MUTEX_UNLOCK(inst);

            if (NULL != request_handler) {
                request_handler(inst, SPM_CTRL_STATUS_MASK & frame->control,
                        frame->command, frame->data, frame->size);
            } else {
                spm_send_error(inst, SPM_INVALID_COMMAND, frame->command);
            }
            return;
        }
    }

    SPM_MUTEX_UNLOCK(inst);

    spm_send_error(inst, SPM_BUSY, frame->command);
}

static void spm_substate_response(struct spm *inst) {
    struct spm_frame *frame = spm_buffer_get_frame(inst);
    struct spm_request *request = inst->tx;

    SPM_MUTEX_LOCK(inst);

    for (unsigned int i = 0; i < SPM_TX_REQUESTS; i++, request++) {
        if ((NULL != request->handler)
                && (frame->command == request->command)) {
            spm_handler_t request_handler = request->handler;
            request->handler = NULL;

            SPM_MUTEX_UNLOCK(inst);

            request_handler(inst, SPM_CTRL_STATUS_MASK & frame->control,
                    frame->command, frame->data, frame->size);
            return;
        }
    }

    SPM_MUTEX_UNLOCK(inst);
}
