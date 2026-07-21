#define _GNU_SOURCE
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <stdlib.h>

#include "main.h"
#include "interface/get_interface.h"
#include "ip/ip_link.h"
#include "arp/arp_link.h"
#include "ip_pthread.h"

extern int raw_sock_fd;
int netmask_num = 3;

// 查找网络段函数
int find_network_segment(unsigned char *ip){
    int i=0,j=get_interface_num();
    for(i=0;i<j;i++){
        if(memcmp(net_interface[i].ip, ip, netmask_num)==0)
            break;
    }
    return i;
}

// IP数据包处理线程函数
void *ip_pthread(void *arg){
    if (arg == NULL) {
        return NULL;
    }
    
    RECV_DATA *recv_data = (RECV_DATA *)arg;
    
    // 转换为unsigned char指针
    unsigned char *data = (unsigned char *)recv_data->data;
    int network_num = find_network_segment(data+30);
    
    if(network_num==get_interface_num()){
        free(recv_data);
        return NULL;
    }
    
    if(data[33]==255){  // 广播数据不转发
        free(recv_data);
        return NULL;
    }
    
    if(strcmp(net_interface[network_num].name,"lo")==0){  // 本地回环不转发
        free(recv_data);
        return NULL;
    }
    
    ARP_LINK * pb = find_arp_from_ip(arp_head, data+30);
    
    if(pb==NULL){  // ARP缓存中没有记录，不转发
        free(recv_data);
        return NULL;
    }
    
    // 转发数据包
    memcpy(data, pb->mac, 6);  // 目的MAC
    memcpy(data+6, net_interface[network_num].mac, 6);  // 源MAC
    
    struct ifreq ethrep;
    strncpy(ethrep.ifr_name, net_interface[network_num].name, IFNAMSIZ);
    if(-1 == ioctl(raw_sock_fd, SIOCGIFINDEX, &ethrep)){
        perror("send_msg-ioctl");
        free(recv_data);
        return NULL;
    }
    
    struct sockaddr_ll sll;
    bzero(&sll, sizeof(sll));
    sll.sll_ifindex = ethrep.ifr_ifindex;
    int ret = sendto(raw_sock_fd, data, recv_data->data_len, 0, (struct sockaddr*)&sll, sizeof(sll));
    if(ret<0){
        perror("sendto");
    }
    
    free(recv_data);
    return NULL;
}