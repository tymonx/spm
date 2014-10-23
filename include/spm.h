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

#ifndef _SPM_H_
#define _SPM_H_

#include "stdint.h"
#include "stddef.h"

#define SPM_THREAD_ENABLE                       0
#define SPM_BUFFER_SIZE                         256
#define SPM_TX_REQUESTS                         16
#define SPM_RX_REQUESTS                         16
#define SPM_TX_TIMEOUT_MS                       300
#define SPM_RX_TIMEOUT_MS                       200

#define SPM_FRAME_HEADER_SIZE                   sizeof(struct spm_frame)
#define SPM_PACKED                              __attribute__((packed))
#define SPM_INLINE                              static inline
#define SPM_ARRAY_SIZE(array)                   (sizeof(array)/sizeof(array[0]))

#define SPM_FRAME_SYNC                          0x3A

#define SPM_CTRL_STATUS_MASK                    0x0F
#define SPM_CTRL_REQUEST                        0x10
#define SPM_CTRL_RESERVED1                      0x20
#define SPM_CTRL_RESERVED2                      0x40
#define SPM_CTRL_RESERVED3                      0x80

#define SPM_OPTIONS_RX                          0x01

#if SPM_THREAD_ENABLE
#define SPM_INIT(io_write, mutex_lock, mutex_unlock)\
    {\
        .mutex = {mutex_lock, mutex_unlock},\
        .state = spm_state_sync,\
        .write = io_write,\
        .buffer = {{0}, 0},\
        .tx = {{0, 0, 0, NULL}},\
        .rx = {{0, 0, 0, NULL}},\
    }
#define SPM_MUTEX_LOCK(inst)\
        if (NULL != (inst)->mutex.lock) (inst)->mutex.lock()
#define SPM_MUTEX_UNLOCK(inst)\
        if (NULL != (inst)->mutex.unlock) (inst)->mutex.unlock()
#else
#define SPM_INIT(io_write)\
    {\
        .state = spm_state_sync,\
        .write = io_write,\
        .buffer = {{0}, 0},\
        .tx = {{0, 0, 0, NULL}},\
        .rx = {{0, 0, 0, NULL}},\
    }
#define SPM_MUTEX_LOCK(inst)
#define SPM_MUTEX_UNLOCK(inst)
#endif /* SPM_THREAD_ENABLE */

enum spm_status {
    SPM_SUCCESS                                 = 0,
    SPM_ERROR_FULL                              = 1,
    SPM_ERROR_SEND                              = 2,
    SPM_ERROR_BUSY                              = 3,
};

enum spm_response_status {
    SPM_OK                                      = 0,
    SPM_BUSY                                    = 1,
    SPM_TIMEOUT                                 = 2,
    SPM_INVALID_CRC                             = 3,
    SPM_INVALID_COMMAND                         = 4,
};

struct spm;

#include "spm_commands.h"

typedef void (*spm_handler_t)(struct spm *inst, unsigned int status,
        unsigned int command, const void *data, unsigned int size);

typedef int (*spm_write_handler_t)(
        const unsigned char *data, unsigned int size);

typedef void (*spm_state_handler_t)(struct spm *inst, unsigned int data);

void spm_state_sync(struct spm *inst, unsigned int sync);

struct spm_mutex {
    void (*lock)(void);
    void (*unlock)(void);
};

struct spm_buffer {
    uint8_t data[SPM_BUFFER_SIZE];
    uint16_t index;
};

struct spm_request {
    uint8_t command;
    uint8_t options;
    uint16_t time_ms;
    spm_handler_t handler;
};

struct spm {
#if SPM_THREAD_ENABLE
    struct spm_mutex mutex;
#endif
    spm_state_handler_t state;
    spm_write_handler_t write;
    struct spm_buffer buffer;
    uint16_t data_count;
    struct spm_request tx[SPM_TX_REQUESTS];
    struct spm_request rx[SPM_RX_REQUESTS];
};

struct spm_frame {
    uint8_t sync;
    uint8_t control;
    uint8_t command;
    uint8_t size;
    uint8_t data[];
}SPM_PACKED;

SPM_INLINE void spm_dispatch(struct spm *inst, unsigned int data) {
    inst->state(inst, data);
}

int spm_send_request(struct spm *inst, spm_handler_t handler,
        unsigned int command, const void *data, unsigned int size);

int spm_send_response(struct spm *inst, unsigned int status,
        unsigned int command, const void *data, unsigned int size);

SPM_INLINE int spm_send_error(struct spm *inst, unsigned int status,
        unsigned int command) {
    return spm_send_response(inst, status, command, NULL, 0);
}

void spm_time_update(struct spm *inst, unsigned int time_ms);

#endif /* _SPM_H_ */
