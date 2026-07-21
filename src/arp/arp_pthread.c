#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arp_pthread.h"
#include "arp_link.h"

// ARP数据包处理线程函数
void *arp_pthread(void *arg)
{
    if (arg == NULL) {
        return NULL;
    }
    
    ARP_LINK *arp_data = (ARP_LINK *)arg;
    
    // 查找ARP缓存表中是否已存在该IP
    ARP_LINK *entry = find_arp_from_ip(arp_head, arp_data->ip);
    if (entry == NULL) {
        // 如果不存在，添加新条目
        entry = (ARP_LINK *)malloc(sizeof(ARP_LINK));
        if (entry != NULL) {
            memcpy(entry->ip, arp_data->ip, 4);
            memcpy(entry->mac, arp_data->mac, 6);
            entry->next = NULL;
            arp_head = inner_arp_link(arp_head, entry);
            printf("添加ARP条目: IP=%u.%u.%u.%u MAC=%02x:%02x:%02x:%02x:%02x:%02x\n",
                   arp_data->ip[0], arp_data->ip[1], arp_data->ip[2], arp_data->ip[3],
                   arp_data->mac[0], arp_data->mac[1], arp_data->mac[2],
                   arp_data->mac[3], arp_data->mac[4], arp_data->mac[5]);
        }
    } else {
        // 如果已存在，更新MAC地址
        memcpy(entry->mac, arp_data->mac, 6);
        printf("更新ARP条目: IP=%u.%u.%u.%u MAC=%02x:%02x:%02x:%02x:%02x:%02x\n",
               arp_data->ip[0], arp_data->ip[1], arp_data->ip[2], arp_data->ip[3],
               arp_data->mac[0], arp_data->mac[1], arp_data->mac[2],
               arp_data->mac[3], arp_data->mac[4], arp_data->mac[5]);
    }
    
    free(arg);  // 释放传入的参数内存
    return NULL;
}

void handle_arp_packet(char *buf, ssize_t len)
{
    if (buf == NULL || len < 42) {
        return;
    }

    const unsigned char *pkt = (const unsigned char *)buf;
    const unsigned char *sender_mac = pkt + 22;
    const unsigned char *sender_ip = pkt + 28;
    ARP_LINK *entry = find_arp_from_ip(arp_head, (unsigned char *)sender_ip);
    if (entry == NULL) {
        entry = (ARP_LINK *)malloc(sizeof(ARP_LINK));
        if (entry != NULL) {
            memcpy(entry->ip, sender_ip, 4);
            memcpy(entry->mac, sender_mac, 6);
            entry->next = NULL;
            arp_head = inner_arp_link(arp_head, entry);
        }
    } else {
        memcpy(entry->mac, sender_mac, 6);
    }
}