# Linux-ARP 路由器项目

## 项目简介

本项目是一个基于 Linux 原始套接字实现的 ARP/IP 路由实验系统，使用 C 语言编写。程序通过 `PF_PACKET` 原始套接字捕获以太网数据包，利用多线程分别处理 ARP 缓存、IP 转发和用户命令输入，支持 IP 黑名单过滤和 ARP 缓存表动态维护。

## 主要功能

- **ARP 缓存管理**：维护 ARP 缓存链表，支持根据 IP 查找 MAC 地址、添加/更新条目、打印和释放链表。
- **IP 过滤（黑名单）**：维护 IP 过滤链表，启动时从 `ip_config` 文件加载，支持命令行动态增删，数据自动保存到配置文件。
- **网络接口探测**：通过 `getifaddrs` 获取所有 IPv4 网络接口的名称、IP、子网掩码、广播地址和 MAC 地址。
- **IP 数据包转发**：接收到的 IP 数据包经黑名单过滤后，根据目的 IP 查找 ARP 缓存获取目标 MAC，并通过对应网络接口转发。
- **多线程架构**：使用 pthread 实现键盘命令线程、ARP 包处理线程和 IP 转发线程的分离。
- **命令行交互**：支持 `arp`、`ip`、`setip a.b.c.d`、`delip a.b.c.d`、`quit`、`help` 等命令。

## 目录结构

```
Linux-ARP/
├── CMakeLists.txt              # CMake 构建配置
├── makefile                    # Makefile 构建配置
├── .gitignore
├── README.md
├── document/
│   └── 路由器项目规划书.pdf
└── src/
    ├── main.c / main.h         # 程序入口，全局变量定义，主循环与线程调度
    ├── arp/
    │   ├── arp_link.c / arp_link.h   # ARP 缓存链表 CRUD
    │   └── arp_pthread.c / arp_pthread.h  # ARP 包处理线程
    ├── ip/
    │   ├── ip_link.c / ip_link.h     # IP 过滤链表 CRUD + 配置文件读写
    │   └── ip_pthread.c / ip_pthread.h  # IP 转发线程
    ├── interface/
    │   ├── get_interface.c / get_interface.h  # 网络接口信息获取
    └── cmd/
        ├── key_thread.c / key_thread.h      # 键盘命令处理线程
```

## 编译与运行

### 环境要求

- Linux 操作系统
- GCC 编译器
- pthread 库

### 使用 CMake 编译

```bash
mkdir build && cd build
cmake ..
make
./router
```

### 使用 Makefile 编译

```bash
make
./router
```

### 清理构建产物

```bash
make clean
```

## 运行说明

程序启动后会执行以下初始化流程：

1. 从 `ip_config` 文件加载 IP 黑名单到内存链表
2. 初始化空的 ARP 缓存表
3. 获取并打印当前网络接口信息
4. 创建键盘命令线程等待用户输入
5. 创建原始套接字进入主循环，捕获网络数据包

### 支持的命令

| 命令 | 说明 |
|------|------|
| `arp` | 打印当前 ARP 缓存表 |
| `ip` | 打印当前 IP 黑名单 |
| `setip a.b.c.d` | 添加 IP 到黑名单 |
| `delip a.b.c.d` | 从黑名单删除 IP |
| `quit` | 退出程序 |
| `help` | 显示帮助信息 |

## 核心工作流程

```
网卡收到以太网帧
       │
       ▼
┌─────────────┐
│  主循环 recvfrom  │
└──────┬──────┘
       │
       ├─ eth_type == ETH_P_ARP ──→ handle_arp_packet() → 更新 ARP 缓存
       │
       └─ eth_type == ETH_P_IP ──→ 检查 IP 黑名单
                │
                ├─ 在黑名单中 → 丢弃
                │
                └─ 不在黑名单 → 创建 IP 处理线程
                              │
                              ▼
                       ip_pthread()
                              │
                              ├─ 目的为本网段 → 丢弃
                              ├─ 广播包 → 丢弃
                              ├─ 回环接口 → 丢弃
                              ├─ ARP 无记录 → 丢弃
                              └─ 有记录 → 修改 MAC 头并通过对应接口 sendto 转发
```

## 依赖关系

各模块之间的头文件引用关系如下：

- `main.c` 依赖所有模块的头文件
- `arp_pthread.c` 依赖 `arp_link.h`
- `ip_pthread.c` 依赖 `main.h`、`get_interface.h`、`ip_link.h`、`arp_link.h`
- `key_thread.c` 依赖 `arp_link.h`、`ip_link.h`、`main.h`
- `get_interface.c` 仅依赖自身头文件
