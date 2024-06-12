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
 * Author: sky
 * Create: 2021-05-22
 * Description: tcp_probe include file
 ******************************************************************************/
#ifndef __TCPPROBE__H
#define __TCPPROBE__H

#define TM_STR_LEN  48

#define TCP_ESTABLISHED 1
#define TCP_SYN_SENT 2
#define TCP_SYN_RECV 3
#define TCP_FIN_WAIT1 4
#define TCP_FIN_WAIT2 5
#define TCP_TIME_WAIT 6
#define TCP_CLOSE 7
#define TCP_CLOSE_WAIT 8
#define TCP_LAST_ACK 9
#define TCP_LISTEN 10
#define TCP_CLOSING 11
#define TCP_NEW_SYN_RECV 12
#define TCP_MAX_STATES 13

#define METRIC_MAX_ENTRIES 1000
#define LINK_MAX_ENTRIES 10000

#define LINK_ROLE_SERVER 0
#define LINK_ROLE_CLIENT 1

#define TCPPROBE_INTERVAL_NS (5000000000)
#define TCPPROBE_CYCLE_SEC (5)

#define MAX_LONG_LINK_FDS_PER_PROC (10)
#define MAX_LONG_LINK_PROCS (20)
#define MAX_PORT_VAL (0xff)

struct long_link_info {
    int fds[MAX_LONG_LINK_FDS_PER_PROC];
    __u8 fd_role[MAX_LONG_LINK_FDS_PER_PROC];
    unsigned int cnt;
};

struct ip {
    union {
        __u32 ip4;
        unsigned char ip6[IP6_LEN];
    };
};

struct metric_key {
    struct ip c_ip;
    struct ip s_ip;
    __u16 s_port;
    __u16 proto;
    __u32 tgid;		// process id
};

struct metric_data {
    char comm[TASK_COMM_LEN];
    __u32 link_num;
    __u64 rx;
    __u64 tx;
    __u32 segs_in;
    __u32 segs_out;
    __u32 total_retrans;
    __u32 lost;
    __u32 srtt;
    __u32 srtt_max;
    __u32 rcv_wnd_min;
    __u32 rcv_wnd_avg;
    __u32 rcv_wnd_max;
    __u32 backlog_drops;
    __u32 sk_drops;
    __u32 md5_hash_drops;
    __u32 filter_drops;
    __u32 ofo_count;
    __u32 tmout;
    __u32 rcvque_full;
    __u32 sndbuf_limit;
    __u32 send_rsts;
    __u32 receive_rsts;
    char role;
};

struct proc_info {
    __u32 tgid;		// process id
    char comm[TASK_COMM_LEN];
    __u64 role : 1;
    __u64 ts : 63;
};

struct link_key {
    union {
        __u32 src_addr;
        unsigned char src_addr6[IP6_LEN];
    };
    union {
        __u32 dst_addr;
        unsigned char dst_addr6[IP6_LEN];
    };
    __u32 family;
    __u16 src_port;
    __u16 dst_port;
};

struct link_data {
    pid_t tgid;		// process id
    char comm[TASK_COMM_LEN];
    __u16 states; /* status after established */
    __u16 role;   /* 0: server 1: client */
    __u64 rx;               // FROM tcp_sock.sk_err
    __u64 tx;               // FROM tcp_sock.sk_err
    __u32 segs_in;          // FROM tcp_sock.segs_in
    __u32 segs_out;         // FROM tcp_sock.segs_out
    __u32 total_retrans;    // FROM tcp_sock.total_retrans
    __u32 lost_out;         // FROM tcp_sock.lost_out
    __u32 srtt;             // FROM tcp_sock.srtt_us
    int sk_err;             // FROM sock.sk_err
    int sk_err_soft;        // FROM sock.sk_err_soft
    __u32 rcv_wnd;          // FROM tcp_sock.rcv_wnd
    __u32 backlog_drops;    // FROM tcp_add_backlog event
    __u32 sk_drops;         // FROM sock.sk_drops
    __u32 md5_hash_drops;   // FROM tcp_v4_inbound_md5_hash event
    __u32 filter_drops;     // FROM tcp_filter event
    __u32 ofo_count;        // FROM tcp_data_queue_ofo event
    __u32 tmout;            // FROM tcp_write_err event
    __u32 rcvque_full;      // FROM sock_rcvqueue_full event
    __u32 sndbuf_limit;     // FROM sock_exceed_buf_limit event
    __u32 send_rsts;        // FROM tcp_send_reset event
    __u32 receive_rsts;     // FROM tcp_receive_reset event
};

#define TCPPROBE_UPDATE_STATS(data, sk, new_state) \
    do { \
        struct tcp_sock *__tcp_sock = (struct tcp_sock *)(sk); \
        (data).rx = _(__tcp_sock->bytes_received); \
        (data).tx = _(__tcp_sock->bytes_acked); \
        (data).segs_in = _(__tcp_sock->segs_in); \
        (data).segs_out = _(__tcp_sock->segs_out); \
        (data).sk_err = _((sk)->sk_err); \
        (data).sk_err_soft = _((sk)->sk_err_soft); \
        (data).states |= (1 << (new_state)); \
        (data).srtt = _(__tcp_sock->srtt_us) >> 3; \
        (data).total_retrans = _(__tcp_sock->total_retrans); \
        (data).lost_out = _(__tcp_sock->lost_out); \
        (data).rcv_wnd = _(__tcp_sock->rcv_wnd); \
    } while (0)

#define __TCPPROBE_INC_EVT_BACKLOG_DROPS(data) __sync_fetch_and_add(&((data).backlog_drops), 1)
#define __TCPPROBE_INC_EVT_MD5_DROPS(data) __sync_fetch_and_add(&((data).md5_hash_drops), 1)
#define __TCPPROBE_INC_EVT_FILTER_DROPS(data) __sync_fetch_and_add(&((data).filter_drops), 1)
#define __TCPPROBE_INC_EVT_OFO(data) __sync_fetch_and_add(&((data).ofo_count), 1)
#define __TCPPROBE_INC_EVT_TMOUT(data) __sync_fetch_and_add(&((data).tmout), 1)
#define __TCPPROBE_INC_EVT_RCVQUE_FULL(data) __sync_fetch_and_add(&((data).rcvque_full), 1)
#define __TCPPROBE_INC_EVT_SNDBUF_LIMIT(data) __sync_fetch_and_add(&((data).sndbuf_limit), 1)
#define __TCPPROBE_INC_EVT_SEND_RSTS(data) __sync_fetch_and_add(&((data).send_rsts), 1)
#define __TCPPROBE_INC_EVT_RECEIVE_RSTS(data) __sync_fetch_and_add(&((data).receive_rsts), 1)

enum TCPPROBE_EVT_E {
    TCPPROBE_EVT_BACKLOG,
    TCPPROBE_EVT_MD5,
    TCPPROBE_EVT_FILTER,
    TCPPROBE_EVT_OFO,
    TCPPROBE_EVT_TMOUT,
    TCPPROBE_EVT_RCVQUE_FULL,
    TCPPROBE_EVT_SNDBUF_LIMIT,
    TCPPROBE_EVT_SEND_RST,
    TCPPROBE_EVT_RECEIVE_RST,
};

#define TCPPROBE_INC_EVT(type, data) \
    do { \
        switch (type) \
        { \
            case TCPPROBE_EVT_BACKLOG: \
                __TCPPROBE_INC_EVT_BACKLOG_DROPS(data); \
                break; \
            case TCPPROBE_EVT_MD5: \
                __TCPPROBE_INC_EVT_MD5_DROPS(data); \
                break; \
            case TCPPROBE_EVT_FILTER: \
                __TCPPROBE_INC_EVT_FILTER_DROPS(data); \
                break; \
            case TCPPROBE_EVT_OFO: \
                __TCPPROBE_INC_EVT_OFO(data); \
                break; \
            case TCPPROBE_EVT_TMOUT: \
                __TCPPROBE_INC_EVT_TMOUT(data); \
                break; \
            case TCPPROBE_EVT_RCVQUE_FULL: \
                __TCPPROBE_INC_EVT_RCVQUE_FULL(data); \
                break; \
            case TCPPROBE_EVT_SNDBUF_LIMIT: \
                __TCPPROBE_INC_EVT_SNDBUF_LIMIT(data); \
                break; \
            case TCPPROBE_EVT_SEND_RST: \
                __TCPPROBE_INC_EVT_SEND_RSTS(data); \
                break; \
            case TCPPROBE_EVT_RECEIVE_RST: \
                __TCPPROBE_INC_EVT_RECEIVE_RSTS(data); \
                break; \
        } \
    } while (0)

#define TCPPROBE_UPDATE_PRCINFO(data, proc_info) \
    do { \
        (data).tgid = (proc_info)->tgid; \
        (data).role = (proc_info)->role; \
        __builtin_memcpy(&(data).comm, &(proc_info)->comm, TASK_COMM_LEN); \
    } while (0)

#endif
