#ifndef MAIN_H
#define MAIN_H

extern int raw_sock_fd;
extern volatile int g_running;

// 定义接收数据结构体，用于传递数据包信息
typedef struct recv_data {
    ssize_t data_len;       // 数据长度
    char data[2048];        // 数据缓冲区
} RECV_DATA;

#endif