/******************************************************************************
 * Copyright (c) Huawei Technologies Co., Ltd. 2021. All rights reserved.
 * iSulad licensed under the Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *     http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v2 for more details.
 * Author: Hubble_Zhu
 * Create: 2021-04-12
 * Description:
 ******************************************************************************/
#ifndef __PROBE_H__
#define __PROBE_H__

#include <stdint.h>
#include <pthread.h>

#include "base.h"
#include "fifo.h"

typedef int (*ProbeMain)(void);

typedef struct {
    char name[MAX_PROBE_NAME_LEN];       // key
    char metaPath[MAX_META_PATH_LEN];

    uint32_t interval;
    ProbeSwitch probeSwitch;
    Fifo *fifo;
    ProbeMain func;

    pthread_t tid;
} Probe;

typedef struct {
    uint32_t size;
    uint32_t probesNum;
    Probe **probes;
} ProbeMgr;

Probe *ProbeCreate(void);
void ProbeDestroy(Probe *probe);

ProbeMgr *ProbeMgrCreate(uint32_t size);
void ProbeMgrDestroy(ProbeMgr *mgr);

int ProbeMgrPut(ProbeMgr *mgr, Probe *probe);
Probe *ProbeMgrGet(ProbeMgr *mgr, const char *probeName);

int ProbeMgrLoadProbes(ProbeMgr *mgr);

extern __thread Probe *g_probe;

#endif

