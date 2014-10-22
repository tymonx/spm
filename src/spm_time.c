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

#include "spm_time.h"

static void spm_timeout(struct spm *inst, struct spm_request *request,
        unsigned int size, unsigned int time_ms, unsigned int timeout_ms) {

    SPM_MUTEX_LOCK(inst);

    for (unsigned int i = 0; i < size; i++, request++) {
        if (NULL != request->handler) {
            if (request->time_ms > timeout_ms) {
                spm_handler_t request_handler = request->handler;
                unsigned int options = request->options;
                unsigned int command = request->command;
                request->handler = NULL;

                SPM_MUTEX_UNLOCK(inst);

                if (SPM_OPTIONS_RX & options) {
                    spm_send_error(inst, SPM_TIMEOUT, command);
                }
                request_handler(inst, SPM_TIMEOUT, command, NULL, 0);

                SPM_MUTEX_LOCK(inst);
            } else {
                request->time_ms += time_ms;
            }
        }
    }

    SPM_MUTEX_UNLOCK(inst);
}

void spm_time_update(struct spm *inst, unsigned int time_ms) {
    spm_timeout(inst, inst->tx, SPM_TX_REQUESTS, time_ms, SPM_TX_TIMEOUT_MS);
    spm_timeout(inst, inst->rx, SPM_RX_REQUESTS, time_ms, SPM_RX_TIMEOUT_MS);
}
