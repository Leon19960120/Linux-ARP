# 定义编译器
CC = gcc

# 定义编译选项
# -Wall, -Wextra: 开启所有常用警告和额外警告
# -g: 生成调试信息
# -fdiagnostics-color=always: 编译输出带颜色，更易读
# -Isrc: 添加头文件搜索路径
CFLAGS = -Wall -Wextra -g -fdiagnostics-color=always -Isrc

# 定义链接器选项
LDFLAGS = -lpthread

# 定义源文件
SOURCES = src/main.c src/ip/ip_link.c src/interface/get_interface.c src/arp/arp_link.c src/cmd/key_thread.c src/arp/arp_pthread.c src/ip/ip_pthread.c

# 从源文件列表中自动生成目标文件列表 (.c -> .o)
OBJECTS = $(SOURCES:.c=.o)

# 定义最终生成的可执行文件名称
EXECUTABLE = router

# 默认目标：当只输入 'make' 时，会执行此目标
all: $(EXECUTABLE)

# 目标：构建可执行文件
# 它依赖于所有的 .o 文件
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

# 目标：编译 .c 文件生成 .o 文件 (通用规则)
# $< 表示第一个前置条件 (即 .c 文件)
# $@ 表示目标 (即 .o 文件)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 定义 .o 文件对头文件的依赖关系
# 如果 ip_link.h 修改了，main.o 和 ip_link.o 将会被重新编译
src/main.o: src/main.h src/ip/ip_link.h src/interface/get_interface.h src/arp/arp_link.h src/ip/ip_pthread.h src/arp/arp_pthread.h src/cmd/key_thread.h
src/ip/ip_link.o: src/ip/ip_link.h
src/arp/arp_link.o: src/arp/arp_link.h
src/interface/get_interface.o: src/interface/get_interface.h
src/ip/ip_pthread.o: src/ip/ip_pthread.h src/ip/ip_link.h src/arp/arp_link.h src/interface/get_interface.h src/main.h
src/arp/arp_pthread.o: src/arp/arp_pthread.h src/arp/arp_link.h
src/cmd/key_thread.o: src/cmd/key_thread.h src/ip/ip_link.h src/arp/arp_link.h src/main.h

# 目标：清除所有生成的文件 (.o 文件和可执行文件)
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ip_filter.conf

# 声明 phony 目标，这些目标不是文件，例如 'all' 和 'clean'
.PHONY: all clean