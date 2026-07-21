
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <string.h>    // For bzero, memcpy
#include <stdlib.h>    // For malloc
#include <strings.h>   // For bzero

#include "ip/ip_link.h"
#include "interface/get_interface.h"
#include "arp/arp_link.h"
#include "main.h"
#include "ip/ip_pthread.h"
#include "arp/arp_pthread.h"
#include "cmd/key_thread.h"

#ifndef BUF_SIZE
#define BUF_SIZE 2048
#endif


volatile int g_running = 1;
int raw_sock_fd;



/******************************************************************
函    数:    int main()
功    能:    程序主函数，初始化系统并创建线程
参    数:    无
返回值:    int - 程序退出状态码，0表示成功
*******************************************************************/
int main()
{
    init_ip_link();              // 初始化 IP 过滤链表，从配置文件读取数据
    init_arp_link();             // 初始化 ARP 缓存链表
    printf_ip_link(ip_head);     // 打印 IP 过滤链表内容
    getinterface();              // 获取并打印网络接口信息

    // 创建键盘输入线程，用于处理用户命令
    pthread_t key_t;             // 键盘线程标识符
    int ret = pthread_create(&key_t, NULL, key_thread, NULL);  // 创建线程
    if (ret != 0) {              // 检查线程创建是否成功
        perror("pthread_create");
        return -1;
    }
    ret = pthread_detach(key_t); // 分离线程，自动回收资源
    if (ret != 0) {              // 检查线程分离是否成功
        perror("pthread_detach");
        return -1;
    }

    // 创建原始套接字，用于捕获网络数据包
    raw_sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (raw_sock_fd < 0) {       // 检查套接字创建是否成功
        perror("socket");
        return -1;
    }

    // 主循环：接收数据包并分发处理
    char buf[BUF_SIZE];
    struct sockaddr saddr;
    socklen_t saddr_len = sizeof(saddr);
    
    while (g_running) {
        ssize_t len = recvfrom(raw_sock_fd, buf, BUF_SIZE, 0, &saddr, &saddr_len);
        if (len < 0) {
            if (g_running) { // 只有在非退出状态下才报错
                perror("recvfrom");
            }
            break;
        }
        
        // 检查数据包类型
        if (len >= 14) { // 至少包含以太网头部
            unsigned short eth_type = ntohs(*(unsigned short *)(buf + 12));
            
            if (eth_type == ETH_P_ARP) {
                // 处理ARP数据包
                handle_arp_packet(buf, len);
            } else if (eth_type == ETH_P_IP) {
                // 检查是否需要过滤
                unsigned char *ip_src = (unsigned char *)(buf + 30);
                if (find_ip(ip_head, ip_src) != NULL) {
                    // IP在黑名单中，过滤掉
                    continue;
                }
                
                // 创建IP数据包处理任务
                RECV_DATA *recv_data = (RECV_DATA *)malloc(sizeof(RECV_DATA));
                if (recv_data != NULL) {
                    recv_data->data_len = len;
                    memcpy(recv_data->data, buf, len);
                    
                    // 创建IP线程处理数据包
                    pthread_t ip_t;
                    ret = pthread_create(&ip_t, NULL, ip_pthread, recv_data);
                    if (ret != 0) {
                        perror("pthread_create");
                        free(recv_data);
                    } else {
                        pthread_detach(ip_t); // 分离线程，自动回收资源
                    }
                }
            }
        }
    }

    // 清理资源
    close(raw_sock_fd);          // 关闭原始套接字
    return 0;                    // 程序正常退出
}