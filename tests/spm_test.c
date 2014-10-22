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
#include <stdio.h>

static int uart1_write(const unsigned char *data, unsigned int size);
static int uart2_write(const unsigned char *data, unsigned int size);

static struct spm spm1 = SPM_INIT(uart1_write);
static struct spm spm2 = SPM_INIT(uart2_write);

static int uart1_write(const unsigned char *data, unsigned int size) {
    printf("Send 1->2: [%d][", size);
    if (size > 0) {
        printf("%02X", data[0]);
    }
    for (unsigned int i = 1; i < size; i++) {
        printf(" %02X", data[i]);
    }
    printf("]\n");
    for (unsigned int i = 0; i < size; i++) {
        spm_dispatch(&spm2, data[i]);
    }
    return SPM_SUCCESS;
}

static int uart2_write(const unsigned char *data, unsigned int size) {
    printf("Send 2->1: [%d][", size);
    if (size > 0) {
        printf("%02X", data[0]);
    }
    for (unsigned int i = 1; i < size; i++) {
        printf(" %02X", data[i]);
    }
    printf("]\n");
    for (unsigned int i = 0; i < size; i++) {
        spm_dispatch(&spm1, data[i]);
    }

    return SPM_SUCCESS;
}

static void spm1_ping_response(struct spm *inst, unsigned int status,
        unsigned int command, const void *data, unsigned int size) {
    (void)inst;
    (void)command;
    (void)data;
    (void)size;

    printf("SPM1: ping response %d\n", status);
}

static void spm1_get_fw_version_response(struct spm *inst, unsigned int status,
        unsigned int command, const void *data, unsigned int size) {
    (void)inst;
    (void)status;
    (void)command;
    (void)size;

    struct spm_get_fw_version_response *ver =
        (struct spm_get_fw_version_response *)data;

    printf("Firmware version: %d.%d.%d.%d\n",
            ver->major, ver->minor, ver->bugfix, ver->build);
}

int main(void) {
    for (unsigned int i = 0; i < 19; i++) {
        spm_send_request(&spm1, spm1_ping_response,
                SPM_PING_CMD, NULL, SPM_PING_RQST_SIZE);
        spm_send_request(&spm1, spm1_get_fw_version_response,
                SPM_GET_FW_VERSION_CMD,
                NULL, SPM_GET_FW_VERSION_RQST_SIZE);
    }

    return 0;
}
