/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * gala-gopher licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: luzhihao
 * Create: 2022-02-22
 * Description: block probe bpf prog
 ******************************************************************************/
#ifndef __BLOCKPROBE__H
#define __BLOCKPROBE__H

enum blk_type_e {
    BLK_TYPE_INVALID = 0,
    BLK_TYPE_DISK = 1,
    BLK_TYPE_PART,
    BLK_TYPE_LVM,
    BLK_TYPE_MAX
};

struct block_key {
    int major;
    int first_minor;
};

struct blk_stats {
    __u64 latency_req_max;      // FROM delta between'blk_account_io_done' and @req->start_time_ns
    __u64 latency_req_last;     // Same as above
    __u64 latency_req_sum;      // Same as above
    __u64 latency_req_jitter;   // Same as above
    __u32 count_latency_req;    // Same as above

    __u64 latency_flush_max;    // FROM delta between 'mq_flush_data_end_io' and @req->start_time_ns
    __u64 latency_flush_last;   // Same as above
    __u64 latency_flush_sum;    // Same as above
    __u64 latency_flush_jitter; // Same as above
    __u32 count_latency_flush;  // Same as above
};

struct blk_sas_stats {
    __u64 count_sas_abort;      // FROM 'sas_task_abort'
};

// Comply with the kernel definition
#define ISCSI_ERR_BASE          1000
enum iscsi_err {
    ISCSI_OK            = 0,

    ISCSI_ERR_DATASN,
    ISCSI_ERR_DATA_OFFSET,
    ISCSI_ERR_MAX_CMDSN,
    ISCSI_ERR_EXP_CMDSN,
    ISCSI_ERR_BAD_OPCODE,
    ISCSI_ERR_DATALEN,
    ISCSI_ERR_AHSLEN,
    ISCSI_ERR_PROTO,
    ISCSI_ERR_LUN,
    ISCSI_ERR_BAD_ITT,
    ISCSI_ERR_CONN_FAILED,
    ISCSI_ERR_R2TSN,
    ISCSI_ERR_SESSION_FAILED,
    ISCSI_ERR_HDR_DGST,
    ISCSI_ERR_DATA_DGST,
    ISCSI_ERR_PARAM_NOT_FOUND,
    ISCSI_ERR_NO_SCSI_CMD,
    ISCSI_ERR_INVALID_HOST,
    ISCSI_ERR_XMIT_FAILED,
    ISCSI_ERR_TCP_CONN_CLOSE,
    ISCSI_ERR_SCSI_EH_SESSION_RST,
    ISCSI_ERR_NOP_TIMEDOUT,

    ISCSI_ERR_MAX
};

struct iscsi_conn_stats {
    __u64 conn_err[ISCSI_ERR_MAX]; // FROM 'iscsi_conn_error_event'
};

struct iscsi_stats {
    __u64 latency_iscsi_max;        // FROM delta between tracepoint 'scsi_dispatch_cmd_done' and @req->start_time_ns
    __u64 latency_iscsi_last;       // Same as above
    __u64 latency_iscsi_sum;        // Same as above
    __u64 latency_iscsi_jitter;     // Same as above
    __u32 count_latency_iscsi;      // Same as above

    __u64 count_iscsi_tmout;        // FROM 'scsi_dispatch_cmd_timeout'
    __u64 count_iscsi_err;          // FROM tracepoint 'scsi_dispatch_cmd_error'
};

#define BLOCKPROBE_INTERVAL_NS      (5000000000)
struct block_data {
    __u64 ts;                       // Period of latency stats
    enum blk_type_e blk_type;       // disk; part; lvm
    char blk_name[DISK_NAME_LEN];
    char disk_name[DISK_NAME_LEN];
    struct blk_stats        blk_stats;
    struct iscsi_stats      scsi_stats;
    struct iscsi_conn_stats conn_stats;
    struct blk_sas_stats    sas_stats;
};

#endif
