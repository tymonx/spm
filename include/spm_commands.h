/*
 INTEL CONFIDENTIAL
 Copyright 2014 Intel Corporation All Rights Reserved.
 The source code contained or described herein and all documents related
 to the source code ("Material") are owned by Intel Corporation or its
 suppliers or licensors. Title to the Material remains with Intel
 Corporation or its suppliers and licensors. The Material may contain
 trade secrets and proprietary and confidential information of Intel
 Corporation and its suppliers and licensors, and is protected by worldwide
 copyright and trade secret laws and treaty provisions. No part of the
 Material may be used, copied, reproduced, modified, published, uploaded,
 posted, transmitted, distributed, or disclosed in any way without
 Intel's prior express written permission.

 No license under any patent, copyright, trade secret or other intellectual
 property right is granted to or conferred upon you by disclosure or
 delivery of the Materials, either expressly, by implication, inducement,
 estoppel or otherwise. Any license under such intellectual property rights
 must be express and approved by Intel in writing.

 Unless otherwise agreed by Intel in writing, you may not remove or alter
 this notice or any other notice embedded in Materials by Intel or
 Intel's suppliers or licensors in any way.
*/

#ifndef _SPM_COMMANDS_H_
#define _SPM_COMMANDS_H_

#include "spm.h"

#define SPM_CMD_FUNC(name)\
    void spm_##name##_cmd(struct spm *inst, unsigned int status,\
            unsigned int command, const void *data, unsigned int size)

/* Ping */
#define SPM_PING_CMD                            0x00
#define SPM_PING_RQST_SIZE                      0
#define SPM_PING_RESP_SIZE                      0

SPM_CMD_FUNC(ping);

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

SPM_CMD_FUNC(get_fw_version);

/* Set byte in shared memory */
#define SPM_SET_BYTE_CMD                        0x02
#define SPM_SET_BYTE_RQST_SIZE                  2
#define SPM_SET_BYTE_RESP_SIZE                  0

struct spm_set_byte_request {
    uint8_t address;
    uint8_t data;
}SPM_PACKED;

SPM_CMD_FUNC(set_byte);

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

SPM_CMD_FUNC(get_byte);

#endif /* _SPM_COMMANDS_H_ */
