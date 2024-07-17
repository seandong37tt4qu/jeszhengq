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
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include "base.h"

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LOG_LEVEL;

typedef struct {
    char logDirectory[MAX_LOG_DIRECTORY_LEN];
    LOG_LEVEL logLevel;
    char bpfPinPath[MAX_PIN_PATH_LEN];
} GlobalConfig;

typedef struct {
    uint32_t interval; // useless, it's just a placeholder
} IngressConfig;

typedef struct {
    uint32_t interval;
    uint32_t timeRange;
} EgressConfig;

typedef struct {
    char broker[MAX_KAFKA_BROKER_LEN];
    char topic[MAX_KAFKA_TOPIC_LEN];
    KafkaSwitch kafkaSwitch;
} KafkaConfig;

typedef struct {
    char name[MAX_PROBE_NAME_LEN];
    ProbeSwitch probeSwitch;
    uint32_t interval;
} ProbeConfig;

typedef struct {
    uint32_t probesNum;
    ProbeConfig *probesConfig[MAX_PROBES_NUM];
} ProbesConfig;

typedef struct {
    char name[MAX_PROBE_NAME_LEN];
    char command[MAX_EXTEND_PROBE_COMMAND_LEN];
    char param[MAX_EXTEND_PROBE_PARAM_LEN];
    char startChkCmd[MAX_EXTEND_PROBE_COMMAND_LEN];
    ProbeStartCheckType startChkType;
    ProbeSwitch probeSwitch;
} ExtendProbeConfig;

typedef struct {
    uint32_t probesNum;
    ExtendProbeConfig *probesConfig[MAX_PROBES_NUM];
} ExtendProbesConfig;

typedef struct  {
    uint32_t maxTablesNum;
    uint32_t maxRecordsNum;
    uint32_t maxMetricsNum;
    uint32_t recordTimeout;
} IMDBConfig;

typedef struct {
    uint16_t port;
    uint16_t on;
} WebServerConfig;

typedef struct {
    GlobalConfig *globalConfig;
    IngressConfig *ingressConfig;
    EgressConfig *egressConfig;
    KafkaConfig *kafkaConfig;
    ProbesConfig *probesConfig;
    ExtendProbesConfig *extendProbesConfig;
    IMDBConfig *imdbConfig;
    WebServerConfig *webServerConfig;
} ConfigMgr;

ConfigMgr *ConfigMgrCreate(void);
void ConfigMgrDestroy(ConfigMgr *mgr);

int ConfigMgrLoad(const ConfigMgr *mgr, const char *confPath);

#endif

