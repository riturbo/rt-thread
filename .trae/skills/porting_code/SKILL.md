---
name: porting_code
description: 移植代码到rtthread
---
# 移植规则
1. 将linux的接口调用替换为rtthread中的实现，如果rtthread中没有直接对应的接口时，进一步分析接口功能与目的，换用其它方式实现。
2. 对于无法移植的部分，请显著在文件中标注。
3. 移植的文件都统一放到对等目录的rtthread目录下。
