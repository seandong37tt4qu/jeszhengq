**测试环境**

- redis-server运行环境：`10.137.16.176` 

  虚机规格：4U4G

  内核版本：4.18.0-147.5.1.6.h545.eulerosv2r9.x86_64

  redis版本:6.2.1

- redis-benchmark客户端测试环境：`10.137.16.161`

  虚机规格：4U4G

  内核版本：4.19.90-2012.5.0.0054.oe1.x86_64

**测试命令**

```shell
## 单客户端连接使用10w请求测试
redis-benchmark -h 10.137.16.176 -p 3746 -t get -d 100 -n 100000 -c 1 -P 1
## 多客户端连接在10w请求时耗时较短，测试结果不稳定，使用100w请求测试
redis-benchmark -h 10.137.16.176 -p 3746 -t get -d 100 -n 1000000 -c 100 -P 1
```



**测试条件**

- 1个redis-server，1个线程
- redis-benchmark 基于长连接2测试

**测试结果**：（5次测试平均）

| 客户端并发连接数 | 总请求数 | 开启探针总耗时（s） | 关闭探针总耗时(s) | 性能损耗比例 | benchmark统计时延(ns) | SLI-bpf统计时延(ns) |
| ---------------- | -------- | ------------------- | ----------------- | ------------ | --------------------- | ------------------- |
| 1                | 10w      | 12.815              | 12.532            | 2.3%         | 128,287               | 30,383              |
| 100              | 100w     | 28.81               | 26.92967          | 7.0%         | 2880,310              | 1249,663            |







