C 语言编写的 minhash 和 lsh，用于练习目的

# 1. 目录结构
> **Makefile** make编译脚本  

> **minhash.c** 主要 minhash 和 lsh 计算函数  

> **minhash.h** minhash 和 lsh 函数声明  

> **hashfactory.c** 哈希结构实现  

> **hashfactory.h** 哈希结构声明  

> **test.c** 主要测试 minhash 和 lsh 的函数  

> **naive.c** Naive 方法，用于生成精确结果对比 lsh 的计算精度

> **gen.c** 生成随机测试数据，放置于 data/userdata.nt

> **data** 放置测试数据和生成结果

>> **eval.pl** ```perl eval.pl``` 对比 lsh.res（lsh计算结果） sig.res（naive精确计算结果），计算精度、召回率和 F 值

>> **userdata.nt** ```make gen; ./gen``` 生成，用于 minhash 和 lsh 的随机测试输入数据

>> **naive.res** ```make naive; ./naive```生成，naive 方法生成的精确结果

>> **sig.res** ```make; ./test``` 生成，存放 minhash 方法生成的签名矩阵

>> **lsh.res** ```make; ./test``` 生成，存放 lsh 处理后的计算结果

# 2. 运行方式
```make gen; ./gen``` 随机生成测试数据

```make test; ./test``` 用 minhash 和 lsh 方法计算结果（必须存在 data/userdata.nt 文件）

```make naive; ./naive``` 用 naive 方法计算精确结果（计算时间较长，必须存在 data/userdata.nt 文件）

```cd data; perl eval.pl``` 计算精确方法对比 lsh 方法的精度、召回率和 F 值

```make clean``` 清理目标文件和可执行文件（不清理 userdata.nt 和 *.res 文件）
