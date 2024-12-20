# 项目介绍
    这是一个RTOS项目，在此主要关注bsp/fvp_base_a55芯片的适配。

## 目录结构
`bsp` 目录主要包含各个芯片的适配及配置文件，本项目主要关注bsp/fvp_base_a55芯片。

`components`目录包括各种驱动框架及系统框架

`include`是系统的各种头文件。

`libcpu`包含了各种cpu架构的适配，本项目只关注aarch64架构。

`src`包含了os的基础代码及libc库。

## 编译方法
1. 进入目标目录`cd bsp/fvp_base_a55`
2. 准备环境变量`source env.sh`
3. 进行编译`scons`
