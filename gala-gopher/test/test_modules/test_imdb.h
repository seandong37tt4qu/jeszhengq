#ifndef __TEST_IMDB_H__
#define __TEST_IMDB_H__

#define TEST_SUITE_IMDB \
    {   \
        .suiteName = "TEST_META",   \
        .suiteMain = TestIMDBMain   \
    }

extern void TestIMDBMain(CU_pSuite suite);

#endif

