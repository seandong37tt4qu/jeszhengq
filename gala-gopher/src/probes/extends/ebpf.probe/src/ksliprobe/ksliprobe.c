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
 * Description: KSLI probe user prog
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#ifdef BPF_PROG_KERN
#undef BPF_PROG_KERN
#endif

#ifdef BPF_PROG_USER
#undef BPF_PROG_USER
#endif

#include "bpf.h"
#include "args.h"
#include "ksliprobe.h"
#include "ksliprobe.skel.h"

#define OO_NAME "ksliprobe"

static volatile sig_atomic_t stop;
static struct probe_params params = {.period = DEFAULT_PERIOD};

static void sig_int(int signo)
{
    stop = 1;
}


static void msg_event_handler(void *ctx, int cpu, void *data, unsigned int size)
{
    struct msg_event_data_t *msg_evt_data = (struct msg_event_data_t *)data;
    unsigned char cli_ip_str[INET6_ADDRSTRLEN];
    const char *protocol;
    ip_str(msg_evt_data->ip_info.family, (unsigned char *)&(msg_evt_data->ip_info.ipaddr), cli_ip_str, INET6_ADDRSTRLEN);
    switch (msg_evt_data->conn_id.protocol) {
        case PROTOCOL_REDIS:
            protocol = "REDIS";
            break;
        default:
            protocol = "unknown";
            break;
    }
    fprintf(stdout,
            "|%s|%d|%d|%s|%s|%d|%s|%llu|%s|%llu|%s|%llu|%u|\n",
            OO_NAME,
            msg_evt_data->conn_id.tgid,
            msg_evt_data->conn_id.fd,
            protocol,
            cli_ip_str,
            ntohs(msg_evt_data->ip_info.port),
            msg_evt_data->max.command,
            msg_evt_data->max.rtt_nsec,
            msg_evt_data->min.command,
            msg_evt_data->min.rtt_nsec,
            msg_evt_data->recent.command,
            msg_evt_data->recent.rtt_nsec,
            msg_evt_data->sample_num);
    (void)fflush(stdout);

    return;
}

static void *msg_event_receiver(void *arg)
{
    int fd = *(int *)arg;
    struct perf_buffer *pb;

    pb = create_pref_buffer(fd, msg_event_handler);
    if (pb == NULL) {
        fprintf(stderr, "Failed to create perf buffer.\n");
        stop = 1;
        return NULL;
    }

    poll_pb(pb, params.period * 1000);

    stop = 1;
    return NULL;
}

static int init_conn_mgt_process(int msg_evt_map_fd)
{
    int err;
    pthread_t msg_evt_hdl_thd;

    // 启动读写消息事件处理程序
    err = pthread_create(&msg_evt_hdl_thd, NULL, msg_event_receiver, (void *)&msg_evt_map_fd);
    if (err != 0) {
        fprintf(stderr, "Failed to create connection read/write message event handler thread.\n");
        return -1;
    }
    printf("Connection read/write message event handler thread successfully started!\n");

    return 0;
}

int main(int argc, char **argv)
{
    int err;

    err = args_parse(argc, argv, "t:", &params);
    if (err != 0) {
        return -1;
    }
    printf("arg parse interval time:%us\n", params.period);

	INIT_BPF_APP(ksliprobe, EBPF_RLIM_LIMITED);
    LOAD(ksliprobe, err);

    if (signal(SIGINT, sig_int) == SIG_ERR) {
        fprintf(stderr, "Can't set signal handler: %d\n", errno);
        goto err;
    }

    // 初始化连接管理程序
    err = init_conn_mgt_process(GET_MAP_FD(ksliprobe, msg_event_map));
    if (err != 0) {
        fprintf(stderr, "Init connection management process failed.\n");
        goto err;
    }

    printf("SLI probe successfully started!\n");

    while (!stop) {
        sleep(params.period);
    }

err:
    UNLOAD(ksliprobe);
    return -err;
}
