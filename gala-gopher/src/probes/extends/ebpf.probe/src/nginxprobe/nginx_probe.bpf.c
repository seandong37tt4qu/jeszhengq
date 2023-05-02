#ifdef BPF_PROG_KERN
#undef BPF_PROG_KERN
#endif
#define BPF_PROG_USER
#include "bpf.h"
#include "nginx_probe.h"

char LICENSE[] SEC("license") = "GPL";
/* 4 LB
    ngx_stream_proxy_connect
    uprobe: 获取client sock addr
        ngx_stream_session_t->connection->sockaddr
    uretprobe: 获取 server sock addr 并关联client

 */

#define HASH_ITEM_CNTS 10240

struct bpf_map_def SEC("maps") para_hs = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(__u64),
    .value_size = sizeof(void **),
    .max_entries = HASH_ITEM_CNTS,
};

struct bpf_map_def SEC("maps") hs = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(struct ip_addr),
    .value_size = sizeof(struct ngx_metric),
    .max_entries = HASH_ITEM_CNTS,
};

static void bpf_copy_ip_addr(struct sockaddr *addr, struct ip_addr *ip)
{
    ip->family = _(addr->sa_family);
    if (ip->family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
        ip->ipaddr.ip4 = _(addr_in->sin_addr.s_addr);
        ip->port = _(addr_in->sin_port);
    } else {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
        ip->port = _(addr_in6->sin6_port);
        bpf_probe_read_user(ip->ipaddr.ip6, IP6_LEN, addr_in6->sin6_addr.in6_u.u6_addr8);
    }
    return;
}

UPROBE(ngx_http_upstream_handler, pt_regs)
{
    __u64 tid = bpf_get_current_pid_tgid();
    struct ngx_event_s *evt = (struct ngx_event_s *)PT_REGS_PARM1(ctx);

    struct ngx_connection_s *c = (struct ngx_connection_s *)_(evt->data);
    if (!c) {
        return;
    }

    struct ngx_http_request_s *r = (struct ngx_http_request_s *)_(c->data);
    if (!r) {
        return;
    }

    struct ngx_http_upstream_s *u = (struct ngx_http_upstream_s *)_(r->upstream);
    if (!u) {
        return;
    }

    c = (struct ngx_connection_s *)_(r->connection);
    if (!c) {
        return;
    }

    struct ngx_metric metric = {0};

    metric.is_l7 = 1;
    struct sockaddr *tmp = _(c->sockaddr);
    bpf_copy_ip_addr(tmp, &metric.src_ip);

    tmp = _(c->local_sockaddr);
    bpf_copy_ip_addr(tmp, &metric.ngx_ip);

    ngx_str_t *p_name;
    bpf_probe_read_user(&p_name, sizeof(void **), &(u->peer.name));
    if (!p_name) {
        return;
    }

    unsigned char *dt;
    bpf_probe_read_user(&dt, sizeof(void **), &(p_name->data));
    if (!dt) {
        return;
    }
    bpf_probe_read_user_str(metric.dst_ip_str, INET6_ADDRSTRLEN, dt);

    bpf_map_update_elem(&hs, &(metric.src_ip), &metric, BPF_ANY);
    return;
}

UPROBE(ngx_stream_proxy_init_upstream, pt_regs)
{
    struct ngx_stream_upstream_s *stream;
    __u64 tid = bpf_get_current_pid_tgid();
    struct ngx_stream_session_s *s = (struct ngx_stream_session_s *)PT_REGS_PARM1(ctx);

    bpf_map_update_elem(&para_hs, &tid, &s, BPF_ANY);
}

URETPROBE(ngx_stream_proxy_init_upstream, pt_regs)
{
    struct ngx_connection_s *conn;
    struct ngx_connection_s *peer_conn;
    struct sockaddr *client_addr;
    struct ngx_metric metric = {0};
    struct ngx_stream_upstream_s *stream;
    ngx_str_t *p_name;

    struct sockaddr *tmp;

    struct ngx_stream_session_s *s;
    struct ngx_stream_session_s **t;

    __u64 tid = bpf_get_current_pid_tgid();
    t = (struct ngx_stream_session_s **)bpf_map_lookup_elem(&para_hs, &tid);
    if (!t) {
        bpf_printk("bpf_map_lookup_elem para_hs tid:%llu failed\n", tid);
        return;
    }

    s = *t;
    conn = _(s->connection);

    tmp = _(conn->sockaddr);
    bpf_copy_ip_addr(tmp, &metric.src_ip);

    tmp = _(conn->local_sockaddr);
    bpf_copy_ip_addr(tmp, &metric.ngx_ip);

    bpf_probe_read_user(&stream, sizeof(void **), &(s->upstream));
    if (!stream) {
        bpf_printk("stream null:%p\n", stream);
        return;
    }

    p_name = _(stream->peer.name);
    if (!p_name) {
        bpf_printk("peer.name null\n");
        return;
    }

    unsigned char *dt = _(p_name->data);
    if (!dt) {
        bpf_printk("name->data null\n");
        return;
    }
    bpf_probe_read_user_str(metric.dst_ip_str, INET6_ADDRSTRLEN, dt);
    bpf_map_update_elem(&hs, &(metric.src_ip), &metric, BPF_ANY);

    // bpf_printk(">>>>>ngx_stream_proxy_init_upstream src_port:%u\n", metric.src_ip.port);
    return;
}

UPROBE(ngx_close_connection, pt_regs)
{
    struct ngx_connection_s *conn = (struct ngx_connection_s *)PT_REGS_PARM1(ctx);
    struct ngx_metric *metric;

    struct ip_addr src_ip = {0};
    struct sockaddr *client_addr;

    client_addr = _(conn->sockaddr);
    bpf_copy_ip_addr(client_addr, &src_ip);
    metric = (struct ngx_metric *)bpf_map_lookup_elem(&hs, &src_ip);
    if (!metric) {
        return;
    }

    metric->is_finish = 1;
    bpf_map_update_elem(&hs, &src_ip, metric, BPF_ANY);
    return;
}
