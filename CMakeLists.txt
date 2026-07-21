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
    src/main.c
    src/arp/arp_link.c
    src/arp/arp_pthread.c
    src/interface/get_interface.c
    src/ip/ip_link.c
    src/ip/ip_pthread.c
    src/cmd/key_thread.c
)

target_include_directories(router PRIVATE src)

# 多线程库
target_link_libraries(router pthread)
