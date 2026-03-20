#define _GNU_SOURCE
// 标准 C 库头文件
#include <stdio.h>      // For perror, printf, fprintf
#include <stdlib.h>     // For exit, _exit
#include <string.h>     // For memset, memcpy, strlen, etc.
#include <unistd.h>     // For close (closing socket)
#include <ifaddrs.h>    // For getifaddrs()

// 系统网络相关的头文件 - 确保全部包含，且顺序建议如此
#include <sys/types.h>  // 
#include <sys/socket.h> // For socket, basic socket types (PF_PACKET, SOCK_RAW)
#include <sys/ioctl.h>  // For ioctl, SIOCGIFFLAGS, SIOCGIFADDR, SIOCGIFNETMASK, SIOCGIFBRDADDR, SIOCGIFHWADDR
#include <net/if.h>     // For struct ifreq, IFF_UP, etc. 
#include <netinet/in.h> // For struct sockaddr_in, htons
#include <arpa/inet.h>  // For inet_ntop, inet_ntoa
#include <net/if_arp.h> // 

// 您自己的头文件，通常放在所有系统头文件之后
#include "get_interface.h" // 包含 MAXINTERFACES 和 INTERFACE 结构体的定义

int interface_num=0;//接口数量
INTERFACE net_interface[MAXINTERFACES];//接口数据

/******************************************************************
函	数:	int get_interface_num()
功	能:	获取接口数量
参	数:	无
*******************************************************************/
int get_interface_num(){
	return interface_num;
}

/******************************************************************
函	数:	void getinterface()
功	能:	获取接口信息
参	数:	无
*******************************************************************/
void getinterface(){
    struct ifaddrs *ifaddr, *ifa;
    int family;

    // 重置接口数量
    interface_num = 0;

    // 获取所有接口信息
    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    // 遍历所有接口
    for (ifa = ifaddr; ifa != NULL && interface_num < MAXINTERFACES; ifa = ifa->ifa_next) {
        // 跳过没有地址的接口
        if (ifa->ifa_addr == NULL)
            continue;

        family = ifa->ifa_addr->sa_family;

        // 只处理 IPv4 接口
        if (family == AF_INET) {
            // 检查接口是否已经处理过（避免重复）
            int exists = 0;
            for (int i = 0; i < interface_num; i++) {
                if (strcmp(net_interface[i].name, ifa->ifa_name) == 0) {
                    exists = 1;
                    break;
                }
            }
            if (exists)
                continue;

            // 获取接口名称
            printf("%s\n", ifa->ifa_name);
            sprintf(net_interface[interface_num].name, "%s", ifa->ifa_name);
            printf("-%d-%s--\n", interface_num, net_interface[interface_num].name);

            // 获取接口标志
            int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
            if (sock_fd >= 0) {
                struct ifreq ifr;
                memset(&ifr, 0, sizeof(ifr));
                strncpy(ifr.ifr_name, ifa->ifa_name, IFNAMSIZ-1);
                
                if (ioctl(sock_fd, SIOCGIFFLAGS, &ifr) == 0) {
                    if (ifr.ifr_flags & IFF_UP) {
                        printf("UP\n");
                        net_interface[interface_num].flag = 1;
                    } else {
                        printf("DOWN\n");
                        net_interface[interface_num].flag = 0;
                    }
                }

                // 获取 IP 地址
                if (ioctl(sock_fd, SIOCGIFADDR, &ifr) == 0) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
                    printf("IP:%s\n", inet_ntoa(addr->sin_addr));
                    memcpy(net_interface[interface_num].ip, &addr->sin_addr.s_addr, 4);
                }

                // 获取子网掩码
                if (ioctl(sock_fd, SIOCGIFNETMASK, &ifr) == 0) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
                    printf("netmask:%s\n", inet_ntoa(addr->sin_addr));
                    memcpy(net_interface[interface_num].netmask, &addr->sin_addr.s_addr, 4);
                }

                // 获取广播地址
                if (ioctl(sock_fd, SIOCGIFBRDADDR, &ifr) == 0) {
                    struct sockaddr_in *addr = (struct sockaddr_in *)&ifr.ifr_addr;
                    printf("br_ip:%s\n", inet_ntoa(addr->sin_addr));
                    memcpy(net_interface[interface_num].br_ip, &addr->sin_addr.s_addr, 4);
                }

                // 获取 MAC 地址
                if (ioctl(sock_fd, SIOCGIFHWADDR, &ifr) == 0) {
                    printf("MAC:%02x:%02x:%02x:%02x:%02x:%02x\n\n",
                           (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                           (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                           (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                           (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                           (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                           (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
                    memcpy(net_interface[interface_num].mac, ifr.ifr_hwaddr.sa_data, 6);
                }

                close(sock_fd);
            }

            interface_num++;
        }
    }

    printf("interface_num=%d\n\n", interface_num);
    freeifaddrs(ifaddr);
}