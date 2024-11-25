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
 * Author: Mr.lu
 * Create: 2021-09-28
 * Description: bpf header
 ******************************************************************************/
#ifndef __GOPHER_LIB_BPF_H__
#define __GOPHER_LIB_BPF_H__


#if !defined( BPF_PROG_KERN ) && !defined( BPF_PROG_USER )

#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <sys/resource.h>
#include "task.h"
#include "elf_reader.h"
#include "util.h"

#define EBPF_RLIM_INFINITY  100*1024*1024 // 100M

static __always_inline int libbpf_print_fn(enum libbpf_print_level level, const char *format, va_list args)
{
    return vfprintf(stderr, format, args);
}

static __always_inline int set_memlock_rlimit(void)
{
    struct rlimit rlim_new = {
        .rlim_cur   = EBPF_RLIM_INFINITY,
        .rlim_max   = EBPF_RLIM_INFINITY,
    };

    if (setrlimit(RLIMIT_MEMLOCK, (const struct rlimit *)&rlim_new) != 0) {
        (void)fprintf(stderr, "Failed to increase RLIMIT_MEMLOCK limit!\n");
        return 0;
    }
    return 1;
}

#define GET_MAP_OBJ(probe_name, map_name) (probe_name##_skel->maps.map_name)
#define GET_MAP_FD(probe_name, map_name) bpf_map__fd(probe_name##_skel->maps.map_name)
#define GET_PROG_FD(prog_name) bpf_program__fd(probe_name##_skel->progs.prog_name)

#define __MAP_SET_PIN_PATH(probe_name, map_name, map_path) \
    do { \
        int ret; \
        struct bpf_map *__map; \
        \
        __map = GET_MAP_OBJ(probe_name, map_name); \
        ret = bpf_map__set_pin_path(__map, map_path); \
        DEBUG("======>SHARE map(" #map_name ") set pin path \"%s\"(ret=%d).\n", map_path, ret); \
    } while (0)

#define __PIN_SHARE_MAP_ALL(probe_name) \
        do { \
            __MAP_SET_PIN_PATH(probe_name, __probe_match_map, __PROBE_MATCH_MAP_PIN_PATH); \
            __MAP_SET_PIN_PATH(probe_name, __task_map, SHARE_MAP_TASK_PATH); \
        } while (0)

#define INIT_BPF_APP(app_name) \
    static char __init = 0; \
    do { \
        if (!__init) { \
            /* Set up libbpf errors and debug info callback */ \
            (void)libbpf_set_print(libbpf_print_fn); \
            \
            /* Bump RLIMIT_MEMLOCK  allow BPF sub-system to do anything */ \
            if (set_memlock_rlimit() == 0) { \
                ERROR("BPF app(" #app_name ") failed to set mem limit.\n"); \
                return -1; \
            } \
            __init = 1; \
        } \
    } while (0)

#define LOAD(probe_name, end) \
    struct probe_name##_bpf *probe_name##_skel = NULL;           \
    struct bpf_link *probe_name##_link[PATH_NUM] __maybe_unused; \
    int probe_name##_link_current = 0;    \
    do { \
        int err; \
        /* Open load and verify BPF application */ \
        probe_name##_skel = probe_name##_bpf__open(); \
        if (!probe_name##_skel) { \
            ERROR("Failed to open BPF " #probe_name " skeleton\n"); \
            goto end; \
        } \
        __PIN_SHARE_MAP_ALL(probe_name); \
        if (probe_name##_bpf__load(probe_name##_skel)) { \
            ERROR("Failed to load BPF " #probe_name " skeleton\n"); \
            goto end; \
        } \
        /* Attach tracepoint handler */ \
        err = probe_name##_bpf__attach(probe_name##_skel); \
        if (err) { \
            ERROR("Failed to attach BPF " #probe_name " skeleton\n"); \
            probe_name##_bpf__destroy(probe_name##_skel); \
            probe_name##_skel = NULL; \
            goto end; \
        } \
        INFO("Succeed to load and attach BPF " #probe_name " skeleton\n"); \
    } while (0)

#define OPEN(probe_name, end, load) \
    struct probe_name##_bpf *probe_name##_skel = NULL;           \
    struct bpf_link *probe_name##_link[PATH_NUM] __maybe_unused; \
    int probe_name##_link_current = 0;    \
    do { \
        if (load) \
        {\
            /* Open load and verify BPF application */ \
            probe_name##_skel = probe_name##_bpf__open(); \
            if (!probe_name##_skel) { \
                ERROR("Failed to open BPF " #probe_name " skeleton\n"); \
                goto end; \
            } \
        }\
    } while (0)
    
#define MAP_SET_PIN_PATH(probe_name, map_name, map_path, load) \
    do { \
        if (load) \
        {\
            __MAP_SET_PIN_PATH(probe_name, map_name, map_path);\
        }\
    } while (0)
    
#define LOAD_ATTACH(probe_name, end, load) \
    do { \
        if (load) \
        {\
            int err; \
            __PIN_SHARE_MAP_ALL(probe_name); \
            if (probe_name##_bpf__load(probe_name##_skel)) { \
                ERROR("Failed to load BPF " #probe_name " skeleton\n"); \
                goto end; \
            } \
            /* Attach tracepoint handler */ \
            err = probe_name##_bpf__attach(probe_name##_skel); \
            if (err) { \
                ERROR("Failed to attach BPF " #probe_name " skeleton\n"); \
                probe_name##_bpf__destroy(probe_name##_skel); \
                probe_name##_skel = NULL; \
                goto end; \
            } \
            INFO("Succeed to load and attach BPF " #probe_name " skeleton\n"); \
        }\
    } while (0)

#define UNLOAD(probe_name) \
    do { \
        int err; \
        if (probe_name##_skel != NULL) { \
            probe_name##_bpf__destroy(probe_name##_skel); \
        } \
        for (int i = 0; i < probe_name##_link_current; i++) { \
            err = bpf_link__destroy(probe_name##_link[i]); \
            if (err < 0) { \
                ERROR("Failed to detach BPF " #probe_name " %d\n", err); \
                break; \
            } \
        } \
    } while (0)

#define ELF_REAL_PATH(binary_file, elf_abs_path, container_id, elf_path, path_num) \
    do { \
        path_num = get_exec_file_path( #binary_file, (const char *)elf_abs_path, #container_id, elf_path, PATH_NUM); \
        if ((path_num) <= 0) { \
            ERROR("Failed to get proc(" #binary_file ") abs_path.\n"); \
            free_exec_path_buf(elf_path, path_num); \
            break; \
        } \
    } while (0)

#define UBPF_ATTACH(probe_name, sec, elf_path, func_name, succeed) \
    do { \
        int err; \
        uint64_t symbol_offset; \
        err = resolve_symbol_infos((const char *)elf_path, #func_name, NULL, &symbol_offset); \
        if (err < 0) { \
            ERROR("Failed to get func(" #func_name ") in(%s) offset.\n", elf_path); \
            succeed = 0; \
            break; \
        } \
        \
        /* Attach tracepoint handler */ \
        probe_name##_link[probe_name##_link_current] = bpf_program__attach_uprobe( \
            probe_name##_skel->progs.ubpf_##sec, false /* not uretprobe */, -1, elf_path, (size_t)symbol_offset); \
        err = libbpf_get_error(probe_name##_link[probe_name##_link_current]); \
        if (err) { \
            ERROR("Failed to attach uprobe(" #sec "): %d\n", err); \
            succeed = 0; \
            break; \
        } \
        INFO("Success to attach uprobe(" #probe_name "): to elf: %s\n", elf_path); \
        probe_name##_link_current += 1; \
        succeed = 1; \
    } while (0)

#define UBPF_RET_ATTACH(probe_name, sec, elf_path, func_name, succeed) \
    do { \
        int err; \
        uint64_t symbol_offset; \
        err = resolve_symbol_infos((const char *)elf_path, #func_name, NULL, &symbol_offset); \
        if (err < 0) { \
            ERROR("Failed to get func(" #func_name ") in(%s) offset.\n", elf_path); \
            succeed = 0; \
            break; \
        } \
        \
        /* Attach tracepoint handler */ \
        probe_name##_link[probe_name##_link_current] = bpf_program__attach_uprobe( \
            probe_name##_skel->progs.ubpf_ret_##sec, true /* uretprobe */, -1, elf_path, (size_t)symbol_offset); \
        err = libbpf_get_error(probe_name##_link[probe_name##_link_current]); \
        if (err) { \
            ERROR("Failed to attach uprobe(" #sec "): %d\n", err); \
            succeed = 0; \
            break; \
        } \
        INFO("Success to attach uretprobe(" #probe_name ") to elf: %s\n", elf_path); \
        probe_name##_link_current += 1; \
        succeed = 1; \
    } while (0)

static __always_inline __maybe_unused struct perf_buffer* create_pref_buffer(int map_fd, perf_buffer_sample_fn cb)
{
    struct perf_buffer_opts pb_opts = {};
    struct perf_buffer *pb;
    int ret;

    pb_opts.sample_cb = cb;
    pb = perf_buffer__new(map_fd, 8, &pb_opts);
    if (pb == NULL){
        fprintf(stderr, "ERROR: perf buffer new failed\n");
        return NULL;
    }
    ret = libbpf_get_error(pb);
    if (ret) {
        fprintf(stderr, "ERROR: failed to setup perf_buffer: %d\n", ret);
        perf_buffer__free(pb);
        return NULL;
    }
    return pb;
}

static __always_inline __maybe_unused void poll_pb(struct perf_buffer *pb, int timeout_ms)
{
    int ret;

    while ((ret = perf_buffer__poll(pb, timeout_ms)) >= 0) {
        ;
    }
    return;
}

#endif
#endif
