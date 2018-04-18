# DES-6rounds-DID
实现6轮DES的差分分析

## 描述

程序代码包括以下四个部分

1.	6轮DES算法的实现，主要用于产生合适的明密文对、最终破解时验证。 (const.h 和 des.h)

2.	差分分析表的生成。(did_table.h)

3.	根据给定密钥，随机生成明密文对，并检查是否符合选择明文攻击的条件。(hack.h)

4.	6轮DES算法的差分分析，以及结果展示。(hack.h 和 main.cpp)

具体的文档可参考 [document.pdf](document.pdf)

## 参考

《分组密码的设计与分析》（第二版）
