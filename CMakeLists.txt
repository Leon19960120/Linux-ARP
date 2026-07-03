cmake_minimum_required(VERSION 3.10)
project(router)

# C 语言标准
set(CMAKE_C_STANDARD 99)

# 只在 GCC/Clang 下开启警告，Windows 自动跳过
if(NOT MSVC)
    add_compile_options(-Wall -Wextra)
endif()

# 所有 C 源文件
add_executable(router
    main.c
    arp_link.c
    arp_pthread.c
    get_interface.c
    ip_link.c
    ip_pthread.c
    key_thread.c
)

# 多线程库
target_link_libraries(router pthread)
