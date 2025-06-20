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
 * Author: wo_cow
 * Create: 2022-4-14
 * Description: ksli probe bpf prog
 ******************************************************************************/
#ifdef BPF_PROG_USER
#undef BPF_PROG_USER
#endif
#define BPF_PROG_KERN
#include "bpf.h"
#include <bpf/bpf_endian.h>
#include "ksliprobe.h"

#define BPF_F_INDEX_MASK        0xffffffffULL
#define BPF_F_CURRENT_CPU       BPF_F_INDEX_MASK

#define MAX_CONN_LEN            8192

char g_license[] SEC("license") = "GPL";

struct bpf_map_def SEC("maps") conn_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(struct conn_key_t),
    .value_size = sizeof(struct conn_data_t),
    .max_entries = MAX_CONN_LEN,
};

struct bpf_map_def SEC("maps") msg_event_map = {
    .type = BPF_MAP_TYPE_PERF_EVENT_ARRAY,
    .key_size = sizeof(u32),
    .value_size = sizeof(u32),
};

static __always_inline void init_conn_key(struct conn_key_t *conn_key, int fd, int tgid)
{
    conn_key->fd = fd;
    conn_key->tgid = tgid;
}

static __always_inline int init_conn_id(struct conn_id_t *conn_id, int fd, int tgid, struct probe_val *val)
{
    struct sockaddr *upeer_sockaddr = (struct sockaddr *)PROBE_PARM2(*val);
    if (upeer_sockaddr == (void *)0) {
        return -1;
    }

    int *upeer_addrlen = (int *)PROBE_PARM3(*val);
    if (upeer_addrlen < 0) {
        return -1;
    }

    conn_id->ip_info.family = _(upeer_sockaddr->sa_family);
    if (conn_id->ip_info.family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)upeer_sockaddr;
        conn_id->ip_info.ipaddr.ip4 = _(addr_in->sin_addr.s_addr);
        conn_id->ip_info.port = _(addr_in->sin_port);
    } else if (conn_id->ip_info.family == AF_INET6) {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)upeer_sockaddr;
        conn_id->ip_info.port = _(addr_in6->sin6_port);
        bpf_probe_read(conn_id->ip_info.ipaddr.ip6, IP6_LEN, addr_in6->sin6_addr.in6_u.u6_addr8);
    } else {
        bpf_printk("ip_str family abnormal.\n");
        return -1;
    }

    conn_id->fd = fd;
    conn_id->tgid = tgid;
    conn_id->ts_nsec = bpf_ktime_get_ns();
    return 0;
}


// 创建服务端 tcp 连接
KPROBE_RET(__sys_accept4, pt_regs, CTX_USER)
{
    int fd = PT_REGS_RC(ctx);
    u32 tgid = bpf_get_current_pid_tgid() >> INT_LEN;
    struct probe_val val;

    long err;

    struct conn_key_t conn_key = {0};
    struct conn_data_t conn_data = {0};

    if (PROBE_GET_PARMS(__sys_accept4, ctx, val, CTX_USER) < 0)
        return;

    if (fd < 0) {
        return;
    }

    init_conn_key(&conn_key, fd, tgid);
    if (init_conn_id(&conn_data.id, fd, tgid, &val) < 0) {
        return;
    }

    conn_data.find_state = MSG_READ;

    err = bpf_map_update_elem(&conn_map, &conn_key, &conn_data, BPF_ANY);
    if (err < 0) {
        //bpf_printk("====[tgid=%u]: connection create failed.\n", tgid);
        return;
    }

    return;
}

// 关闭 tcp 连接
KPROBE_RET(__close_fd, pt_regs, CTX_USER)
{
    int fd;
    u32 tgid = bpf_get_current_pid_tgid() >> INT_LEN;
    struct probe_val val;

    struct conn_key_t conn_key = {0};
    struct conn_data_t *conn_data;

    if (PROBE_GET_PARMS(__close_fd, ctx, val, CTX_USER) < 0)
        return;

    if (PT_REGS_RC(ctx) < 0)
        return;

    fd = (int)PROBE_PARM2(val);
    init_conn_key(&conn_key, fd, tgid);
    conn_data = (struct conn_data_t *)bpf_map_lookup_elem(&conn_map, &conn_key);
    if (conn_data == (void *)0) {
        return;
    }
    bpf_map_delete_elem(&conn_map, &conn_key);

    return;
}

static __always_inline void parse_msg_to_redis_cmd(char msg_char, int *j, char *command, unsigned short *find_state)
{
    switch (*find_state) {
        case FIND0_MSG_START:
            if (msg_char == '*') {
                *find_state = FIND1_PARM_NUM;
            } else {
                *find_state = FIND_MSG_ERR_STOP;
            }
            break;
            
        case FIND1_PARM_NUM:
            if ((msg_char >='0' && msg_char <= '9') || msg_char == '\r' || msg_char == '\n') {
                ;
            } else if (msg_char == '$') {
                *find_state = FIND2_CMD_LEN;
            } else {
                *find_state = FIND_MSG_ERR_STOP;
            }
            break;
        case FIND2_CMD_LEN:
            if ((msg_char >='0' && msg_char <= '9') || msg_char == '\r') {
                ;
            } else if (msg_char == '\n') {
                *find_state = FIND3_CMD_STR;
            } else {
                *find_state = FIND_MSG_ERR_STOP;
            }
            break;
        case FIND3_CMD_STR:
            if (msg_char >= 'a') 
                msg_char = msg_char - ('a'-'A');
            if (msg_char >= 'A' && msg_char <= 'Z') {
                command[*j] = msg_char;
                *j = *j + 1;
            } else if (msg_char == '\r' || msg_char == '\n') {
                *find_state = FIND_MSG_OK_STOP;
            } else {
                *find_state = 15;
            }
            break;
        case FIND_MSG_OK_STOP:
        case FIND_MSG_ERR_STOP:
            break;
        default:
            break;
    }
    
    return;
}

static __always_inline int identify_protocol_redis(char *msg, char *command)
{
    int j = 0;
    unsigned short find_state = FIND0_MSG_START;

#pragma clang loop unroll(full)
    for (int i = 0; i < MAX_COMMAND_REQ_SIZE - 2; i++) {
        // TODO: 循环内i不能被改变所以先简单写下
       parse_msg_to_redis_cmd(msg[i], &j, command, &find_state);
    }
    if (find_state != FIND_MSG_OK_STOP) {
        return SLI_ERR;
    }
    return SLI_OK;
}

static __always_inline int parse_req(struct conn_data_t *conn_data, const unsigned int count, const char *buf)
{
    volatile u32 copy_size;
    long err;
    char msg[MAX_COMMAND_REQ_SIZE] = {0};

    conn_data->ts_nsec_req = bpf_ktime_get_ns();

    copy_size = count < MAX_COMMAND_REQ_SIZE ? count : (MAX_COMMAND_REQ_SIZE - 1);
    err = bpf_probe_read(msg, copy_size & MAX_COMMAND_REQ_SIZE, buf);
    if (err < 0) {
        bpf_printk("parse_req read buffer failed.\n");
        return PROTOCOL_UNKNOWN;
    }

    // 解析请求中的command，确认协议
    if (identify_protocol_redis(msg, conn_data->recent.command) == SLI_OK) {
        return PROTOCOL_REDIS;
    }

    return PROTOCOL_UNKNOWN;
}

#define __PERIOD ((u64)5 * 1000000000) // 5s
static __always_inline void periodic_report(u64 ts_nsec, struct conn_data_t *conn_data, struct pt_regs *ctx)
{
    long err;
    if (ts_nsec - conn_data->last_report_ts_nsec >= __PERIOD) {
        struct msg_event_data_t msg_evt_data = {0};
        msg_evt_data.conn_id = conn_data->id;
        msg_evt_data.sample_num = conn_data->sample_num;
        msg_evt_data.ip_info = conn_data->id.ip_info;
        msg_evt_data.max = conn_data->max;
        msg_evt_data.min = conn_data->min;
        msg_evt_data.recent = conn_data->recent;
        err = bpf_perf_event_output(ctx, &msg_event_map, BPF_F_CURRENT_CPU,
                                    &msg_evt_data, sizeof(struct msg_event_data_t));
        if (err < 0) {
            bpf_printk("message event sent failed.\n");
        }
        conn_data->sample_num = 0;
        conn_data->last_report_ts_nsec = ts_nsec;
    }
    return;
}


static __always_inline void process_rdwr_msg(int fd, const char *buf, const unsigned int count, enum msg_event_rw_t rw_type,
                                             struct pt_regs *ctx)
{
    u32 tgid = bpf_get_current_pid_tgid() >> INT_LEN;
    struct conn_key_t conn_key = {0};
    struct conn_data_t *conn_data;
    u64 ts_nsec = bpf_ktime_get_ns();

    init_conn_key(&conn_key, fd, tgid);
    conn_data = (struct conn_data_t *)bpf_map_lookup_elem(&conn_map, &conn_key);
    if (conn_data == (void *)0) {
        return;
    }

    if (rw_type != conn_data->find_state) {
        return;
    }

    if (rw_type == MSG_READ) {
        enum conn_protocol_t protocol = parse_req(conn_data, count, buf);
        if (protocol == PROTOCOL_UNKNOWN) {
            return;
        }
        if (conn_data->sample_num == 0) {
            conn_data->id.protocol = protocol;
        }
        conn_data->ts_nsec_req = ts_nsec;
        conn_data->find_state = MSG_WRITE;
    } else {
        // TODO:需要检查响应格式吗？
        conn_data->find_state = MSG_READ;
        conn_data->recent.rtt_nsec = ts_nsec - conn_data->ts_nsec_req;
        if (conn_data->sample_num == 0) {
            conn_data->max = conn_data->recent;
            conn_data->min = conn_data->recent;
        } else {
            if (conn_data->recent.rtt_nsec > conn_data->max.rtt_nsec) {
                conn_data->max = conn_data->recent;
            } else if (conn_data->recent.rtt_nsec < conn_data->min.rtt_nsec) {
                conn_data->min = conn_data->recent;
            }
        }

        conn_data->sample_num++;
        // 周期上报
        periodic_report(ts_nsec, conn_data, ctx);
        __builtin_memset(&(conn_data->recent), 0x0, sizeof(conn_data->recent));
    }

    return;
}

// 跟踪连接 read 读消息
KPROBE_RET(ksys_read, pt_regs, CTX_USER)
{
    int fd;
    u32 tgid __maybe_unused = bpf_get_current_pid_tgid() >> INT_LEN;
    int count = PT_REGS_RC(ctx);
    char *buf;
    struct probe_val val;
    int err;

    err = PROBE_GET_PARMS(ksys_read, ctx, val, CTX_USER);
    if (err < 0) {
        return;
    }

    if (count <= 0) {
        return;
    }

    fd = (int)PROBE_PARM1(val);
    buf = (char *)PROBE_PARM2(val);

    process_rdwr_msg(fd, buf, count, MSG_READ, ctx);

    return;
}

// 跟踪连接 write 写消息
KPROBE_RET(ksys_write, pt_regs, CTX_USER)
{
    int fd;
    u32 tgid __maybe_unused = bpf_get_current_pid_tgid() >> INT_LEN;
    int count = PT_REGS_RC(ctx);
    char *buf;
    struct probe_val val;
    int err;

    err = PROBE_GET_PARMS(ksys_write, ctx, val, CTX_USER);
    if (err < 0) {
        return;
    }

    if (count <= 0) {
        return;
    }

    fd = (int)PROBE_PARM1(val);
    buf = (char *)PROBE_PARM2(val);

    process_rdwr_msg(fd, buf, count, MSG_WRITE, ctx);

    return;
}