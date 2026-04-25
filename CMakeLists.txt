cmake_minimum_required(VERSION 3.10)
project(router)

# C 语言标准
set(CMAKE_C_STANDARD 99)

# 开启所有警告
add_compile_options(-Wall -Wextra)

# 把你所有的 .c 文件全部列在这里
add_executable(router
    main.c
    arp_link.c
    arp_pthread.c
    get_interface.c
    ip_link.c
    ip_pthread.c
    key_thread.c
)

# Linux 网络多线程必须链接的库
target_link_libraries(router pthread)
