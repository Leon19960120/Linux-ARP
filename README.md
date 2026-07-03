# Linux-ARP Router Project

## Overview
This project is a Linux-based router experiment system written in C. It features ARP cache management, IP filtering, network interface information retrieval, and multi-threaded packet processing. The code is modular, with separate files for ARP/IP list management, thread handling, and interface operations.

## Features
- ARP cache linked list management (add, delete, search, print)
- IP filter linked list management
- Multi-threaded packet processing using pthreads
- Network interface information retrieval
- Configurable via CMake or Makefile

## Build & Run
```bash
# Using CMake
mkdir build && cd build
cmake ..
make
./router

# Or using Makefile
make
./router
```

## File Structure
- `main.c` - Program entry, thread creation, system initialization
- `arp_link.*` - ARP cache linked list logic
- `ip_link.*` - IP filter linked list logic
- `get_interface.*` - Network interface info
- `arp_pthread.c`, `ip_pthread.c`, `key_thread.c` - Thread logic

## Requirements
- Linux OS
- GCC
- pthread library

---

# Linux-ARP 路由器项目

## 项目简介
本项目为基于C语言的Linux路由器实验系统，实现了ARP缓存管理、IP过滤、网络接口信息获取及多线程数据包处理。代码结构清晰，功能模块独立，便于学习和扩展。

## 主要功能
- ARP缓存链表管理（增删查打印）
- IP过滤链表管理
- 基于pthread的多线程数据包处理
- 网络接口信息获取
- 支持CMake和Makefile构建

## 编译与运行
```bash
# 使用CMake
mkdir build && cd build
cmake ..
make
./router

# 或直接使用Makefile
make
./router
```

## 目录结构
- `main.c` - 主程序入口，线程创建与系统初始化
- `arp_link.*` - ARP链表管理
- `ip_link.*` - IP过滤链表管理
- `get_interface.*` - 网络接口信息
- `arp_pthread.c`、`ip_pthread.c`、`key_thread.c` - 线程处理逻辑

## 依赖环境
- Linux操作系统
- GCC编译器
- pthread库
