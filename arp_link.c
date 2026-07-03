#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "arp_link.h"

// 全局ARP链表头指针（在头文件中通过 extern 声明）
ARP_LINK *arp_head = NULL;

/******************************************************************
函  数: void init_arp_link()
功  能: 初始化ARP缓存链表
参  数: 无
返回值: 无
*******************************************************************/
void init_arp_link()
{
    // 初始化ARP链表头为NULL
    arp_head = NULL;
}

/******************************************************************
函  数: ARP_LINK *inner_arp_link(ARP_LINK *head, ARP_LINK* p)
功  能: 插入ARP链表（尾插法）
参  数: ARP_LINK *head - ARP链表头
       ARP_LINK *p    - 待插入节点
返回值: ARP_LINK *head - 更新后的链表头
*******************************************************************/
ARP_LINK *inner_arp_link(ARP_LINK *head, ARP_LINK *p)
{
    if (p == NULL) {
        return head;
    }

    // 将新节点的 next 指针置为 NULL
    p->next = NULL;

    // 链表为空时，新节点作为头节点
    if (head == NULL) {
        return p;
    }

    // 尾插法：找到链表末尾
    ARP_LINK *cur = head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = p;

    return head;
}

/******************************************************************
函  数: void printf_arp_link(ARP_LINK *head)
功  能: 打印ARP链表
参  数: ARP_LINK *head - ARP链表头
返回值: 无
*******************************************************************/
void printf_arp_link(ARP_LINK *arp_head)
{
    ARP_LINK *cur = arp_head;
    printf("ARP Table:\n");
    printf("IP Address\t\tMAC Address\n");
    printf("=====================================\n");
    while (cur != NULL) {
        printf("%u.%u.%u.%u\t\t",
               cur->ip[0], cur->ip[1], cur->ip[2], cur->ip[3]);
        printf("%02X:%02X:%02X:%02X:%02X:%02X\n",
               cur->mac[0], cur->mac[1], cur->mac[2],
               cur->mac[3], cur->mac[4], cur->mac[5]);
        cur = cur->next;
    }
    printf("=====================================\n");
}

/******************************************************************
函  数: ARP_LINK *find_arp_from_ip(ARP_LINK *head, unsigned char *ip)
功  能: 根据IP查找MAC
参  数: ARP_LINK *head - ARP链表头
       unsigned char *ip - IP地址（4字节）
返回值: ARP_LINK * 查找到的节点（若未找到则 NULL）
*******************************************************************/
ARP_LINK *find_arp_from_ip(ARP_LINK *arp_head, unsigned char *ip)
{
    if (ip == NULL) {
        return NULL;
    }

    ARP_LINK *cur = arp_head;
    while (cur != NULL) {
        if (memcmp(cur->ip, ip, 4) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

/******************************************************************
函  数: void free_arp_link(ARP_LINK *head)
功  能: 释放ARP链表
参  数: ARP_LINK *head - ARP链表头
返回值: 无
*******************************************************************/
void free_arp_link(ARP_LINK *arp_head)
{
    ARP_LINK *tmp;
    while (arp_head != NULL) {
        tmp = arp_head;
        arp_head = arp_head->next;
        free(tmp);
    }
}