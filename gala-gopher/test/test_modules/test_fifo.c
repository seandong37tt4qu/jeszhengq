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
 * Author: Hubble_Zhu
 * Create: 2021-04-26
 * Description: provide gala-gopher test
 ******************************************************************************/
#include <stdint.h>
#include <CUnit/Basic.h>

#include "fifo.h"
#include "test_fifo.h"

#define FIFO_MGR_SIZE 1024
#define FIFO_SIZE  1024

static void TestFifoMgrCreate(void)
{
    FifoMgr *mgr = FifoMgrCreate(FIFO_MGR_SIZE);

    CU_ASSERT(mgr != NULL);
    CU_ASSERT(mgr->fifos != NULL);
    CU_ASSERT(mgr->size == FIFO_MGR_SIZE);
    CU_ASSERT(mgr->fifoNum == 0);
    FifoMgrDestroy(mgr);
}

static void TestFifoMgrAdd(void)
{
    uint32_t ret = 0;
    FifoMgr *mgr = FifoMgrCreate(FIFO_MGR_SIZE);
    Fifo *fifo = FifoCreate(FIFO_SIZE);

    ret = FifoMgrAdd(mgr, fifo);
    CU_ASSERT(ret == 0);
    CU_ASSERT(mgr->fifoNum == 1);
    CU_ASSERT(mgr->fifos[0] == fifo);
    FifoDestroy(fifo);
    FifoMgrDestroy(mgr);
}

static void TestFifoCreate(void)
{
    Fifo *fifo = FifoCreate(FIFO_SIZE);

    CU_ASSERT(fifo != NULL);
    CU_ASSERT(fifo->buffer != NULL);
    CU_ASSERT(fifo->in == 0);
    CU_ASSERT(fifo->out == 0);
    CU_ASSERT(fifo->size == FIFO_SIZE);
    FifoDestroy(fifo);
}

static void TestFifoPut(void)
{
    uint32_t ret = 0;
    uint32_t elem = 1;
    Fifo *fifo = FifoCreate(FIFO_SIZE);

    CU_ASSERT(fifo != NULL);
    ret = FifoPut(fifo, &elem);
    CU_ASSERT(ret == 0);
    CU_ASSERT(fifo->in == 1);
    FifoDestroy(fifo);
}

static void TestFifoGet(void)
{
    uint32_t ret = 0;
    uint32_t elem = 1;
    uint32_t *elemP = NULL;
    Fifo *fifo = FifoCreate(FIFO_SIZE);

    CU_ASSERT(fifo != NULL);
    ret = FifoPut(fifo, &elem);
    CU_ASSERT(ret == 0);
    ret = FifoGet(fifo, (void **) &elemP);
    CU_ASSERT(ret == 0);
    CU_ASSERT(fifo->out == 1);
    FifoDestroy(fifo);
}

void TestFifoMain(CU_pSuite suite)
{
    CU_ADD_TEST(suite, TestFifoMgrCreate);
    CU_ADD_TEST(suite, TestFifoMgrAdd);
    CU_ADD_TEST(suite, TestFifoCreate);
    CU_ADD_TEST(suite, TestFifoPut);
    CU_ADD_TEST(suite, TestFifoGet);
}

