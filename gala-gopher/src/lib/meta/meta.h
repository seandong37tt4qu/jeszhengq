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
#ifndef __META_H__
#define __META_H__

#include <stdint.h>
#include "base.h"

typedef struct {
    char description[MAX_FIELD_DESCRIPTION_LEN];
    char type[MAX_FIELD_TYPE_LEN];
    char name[MAX_FIELD_NAME_LEN];
} Field;

typedef struct {
    char name[MAX_MEASUREMENT_NAME_LEN];

    uint32_t fieldsNum;
    Field fields[MAX_FIELDS_NUM];
} Measurement;

typedef struct {
    uint32_t size;
    uint32_t measurementsNum;
    Measurement **measurements;
} MeasurementMgr;

MeasurementMgr *MeasurementMgrCreate(uint32_t size);
void MeasurementMgrDestroy(MeasurementMgr *mgr);

int MeasurementMgrLoad(const MeasurementMgr *mgr, const char *metaDir);
int MeasurementMgrLoadSingleMeta(MeasurementMgr *mgr, const char *metaPath);

#endif

