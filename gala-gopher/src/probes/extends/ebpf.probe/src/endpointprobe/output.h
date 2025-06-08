/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 */
#ifndef __OUTPUT_H__
#define __OUTPUT_H__

#ifdef BPF_PROG_KERN

#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

#include "endpoint.h"

#define BPF_F_INDEX_MASK    0xffffffffULL
#define BPF_F_ALL_CPU   BPF_F_INDEX_MASK

#define __ENDPOINT_MAX (10 * 1024)
// Used to identifies the TCP listen/connect and UDP sock object.
struct bpf_map_def SEC("maps") endpoint_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(struct sock *),
    .value_size = sizeof(struct endpoint_v),
    .max_entries = __ENDPOINT_MAX,
};

#define __PERF_OUT_MAX (64)
struct bpf_map_def SEC("maps") output = {
    .type = BPF_MAP_TYPE_PERF_EVENT_ARRAY,
    .key_size = sizeof(u32),
    .value_size = sizeof(u32),
    .max_entries = __PERF_OUT_MAX,
};

// Data collection period
struct bpf_map_def SEC("maps") period_map = {
    .type = BPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(u32),    // const value 0
    .value_size = sizeof(u64),  // period time as second
    .max_entries = 1,
};

#define __PERIOD ((u64)30 * 1000000000)
static __always_inline u64 get_period()
{
    u32 key = 0;
    u64 period = __PERIOD;

    u64 *value = (u64 *)bpf_map_lookup_elem(&period_map, &key);
    if (value)
        period = *value;

    return period; // units from second to nanosecond
}

static __always_inline int create_sock_map(struct sock *sk, enum endpoint_t type, u32 tgid)
{
    struct endpoint_v val = {0};
    val.type = type;
    val.tgid = tgid;
    return bpf_map_update_elem(&endpoint_map, &sk, &val, BPF_ANY);
}

static __always_inline __maybe_unused int delete_sock_map(struct sock *sk)
{
    return bpf_map_delete_elem(&endpoint_map, &sk);
}

static __always_inline struct endpoint_v* get_endpoint_val(struct sock *sk)
{
    return bpf_map_lookup_elem(&endpoint_map, &sk);
}

static __always_inline void init_ip(struct ip *ip_addr, struct sock *sk)
{
    int family = _(sk->sk_family);
    if (family == AF_INET) {
        ip_addr->ip.ip4 = _(sk->sk_rcv_saddr);
    } else if (family == AF_INET6) {
        bpf_probe_read(ip_addr->ip.ip6, IP6_LEN, &sk->sk_v6_rcv_saddr);
    }
    ip_addr->family = family;

    return;
}

static __always_inline void __do_report(void *ctx, struct endpoint_val_t *val)
{
    (void)bpf_perf_event_output(ctx, &output, BPF_F_ALL_CPU, val, sizeof(struct endpoint_val_t));
}

static __always_inline void report(void *ctx, struct endpoint_val_t *val, u32 new_entry)
{
    if (new_entry) {
        __do_report(ctx, val);
    } else {
        u64 ts = bpf_ktime_get_ns();
        u64 period = get_period();
        if ((ts > val->ts) && ((ts - val->ts) < period)) {
            return;
        }
        val->ts = ts;
        __do_report(ctx, val);
    }

    __builtin_memset(&(val->ep_stats), 0x0, sizeof(val->ep_stats));
}

#define ATOMIC_INC_EP_STATS(ep_val, metric, DELTA) \
    __sync_fetch_and_add(&((ep_val)->ep_stats.stats[metric]), DELTA)

#endif

#endif
