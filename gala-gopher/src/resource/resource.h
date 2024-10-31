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
#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "base.h"
#include "config.h"
#include "imdb.h"

#include "probe.h"
#include "extend_probe.h"
#include "meta.h"
#include "fifo.h"

#include "kafka.h"

#include "ingress.h"
#include "egress.h"

#include "web_server.h"

typedef struct {
    // config
    ConfigMgr *configMgr;

    // in-memory database
    IMDB_DataBaseMgr *imdbMgr;

    // inner component
    ProbeMgr *probeMgr;
    ExtendProbeMgr *extendProbeMgr;

    MeasurementMgr *mmMgr;
    FifoMgr *fifoMgr;

    // outer component
    KafkaMgr *kafkaMgr;

    // thread handler
    IngressMgr *ingressMgr;
    EgressMgr *egressMgr;

    // web server
    WebServer *webServer;

    // ctl server
    pthread_t ctl_tid;

    // keep-live timer id
    timer_t keeplive_timer;
} ResourceMgr;

ResourceMgr *ResourceMgrCreate(void);
void ResourceMgrDestroy(ResourceMgr *resourceMgr);

int ResourceMgrInit(ResourceMgr *resourceMgr);
void ResourceMgrDeinit(ResourceMgr *resourceMgr);

#endif

