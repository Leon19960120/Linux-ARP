# 定义编译器
CC = gcc

# 定义编译选项
# -Wall, -Wextra: 开启所有常用警告和额外警告
# -g: 生成调试信息
# -fdiagnostics-color=always: 编译输出带颜色，更易读
CFLAGS = -Wall -Wextra -g -fdiagnostics-color=always

# 定义链接器选项 
LDFLAGS = -lpthread

# 定义源文件
SOURCES = main.c ip_link.c get_interface.c arp_link.c key_thread.c arp_pthread.c ip_pthread.c

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
main.o: ip_link.h arp_link.h get_interface.h main.h ip_pthread.h arp_pthread.h key_thread.h
ip_link.o: ip_link.h
arp_link.o: arp_link.h
get_interface.o: get_interface.h
ip_pthread.o: ip_pthread.h ip_link.h arp_link.h get_interface.h main.h
arp_pthread.o: arp_pthread.h arp_link.h
key_thread.o: key_thread.h ip_link.h arp_link.h main.h

# 目标：清除所有生成的文件 (.o 文件和可执行文件)
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) ip_filter.conf

# 声明 phony 目标，这些目标不是文件，例如 'all' 和 'clean'
.PHONY: all clean