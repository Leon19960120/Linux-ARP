// void init_ip_link()
// {
//     File *ip_config = NULL;
//     ip_config =open(ip_config_name, O_RDWR);
//     if(ip_config =NULL ){
//         perror("open ip_config");
//         exit(-1);
//     }
//     // 初始化IP链表头指针
//     ip_head = NULL;
// }

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memcmp and memcpy
#include "ip_link.h"

// 定义配置文件名
#define IP_FILTER_CONFIG_FILE "ip_config"

// 全局IP链表头指针
// 在头文件中已通过 extern 声明，这里进行定义和初始化
IP_LINK *ip_head = NULL;

/******************************************************************
函  数: IP_LINK *inner_ip_link(IP_LINK *head, IP_LINK* p)
功  能: 插入ip过滤链表（尾插法）
参  数: IP_LINK *head - ip过滤链表头
       IP_LINK* p    - 待插入节点
返回值: IP_LINK *head - 更新后的链表头
*******************************************************************/
IP_LINK *inner_ip_link(IP_LINK *head, IP_LINK* p)
{
    if (p == NULL) {
        fprintf(stderr, "Error: inner_ip_link received NULL node pointer.\n");
        return head;
    }

    // 新节点总是作为末尾，所以其next指针为NULL
    p->next = NULL;

    // 如果链表为空，新节点就是头节点
    if (head == NULL) {
        return p;
    }

    // 找到链表末尾
    IP_LINK *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    // 将新节点插入到末尾
    current->next = p;

    return head;
}

/******************************************************************
函  数: void printf_ip_link(IP_LINK *head)
功  能: 打印ip过滤链表
参  数: IP_LINK *head - ip过滤链表头
返回值: 无
*******************************************************************/
void printf_ip_link(IP_LINK *head)
{
    IP_LINK *current = head;
    int count = 0;

    printf("\n============= IP Filter List =============\n");
    if (head == NULL) {
        printf("IP filter list is empty.\n");
    } else {
        while (current != NULL) {
            printf("%d.%d.%d.%d\n",
                   current->ip[0], current->ip[1], current->ip[2], current->ip[3]);
            current = current->next;
            count++;
        }
    }
    printf("==========================================\n");
    printf("Total filtered IPs: %d\n\n", count);
}

/******************************************************************
函  数: IP_LINK *find_ip(IP_LINK *head, unsigned char *ip)
功  能: 查找ip过滤链表
参  数: IP_LINK *head       - ip过滤链表头
       unsigned char *ip - 待查找IP地址（4字节）
返回值: IP_LINK *找到的节点（若未找到则 NULL）
*******************************************************************/
IP_LINK *find_ip(IP_LINK *head, unsigned char *ip)
{
    if (ip == NULL) {
        fprintf(stderr, "Error: find_ip received NULL IP pointer.\n");
        return NULL;
    }

    IP_LINK *current = head;
    while (current != NULL) {
        // 使用 memcmp 比较 IP 地址的四个字节
        if (memcmp(current->ip, ip, 4) == 0) {
            return current; // 找到匹配的IP
        }
        current = current->next;
    }
    return NULL; // 未找到
}

/******************************************************************
函  数: void free_ip_link(IP_LINK *head)
功  能: 释放ip过滤链表
参  数: IP_LINK *head - ip过滤链表头
返回值: 无
*******************************************************************/
void free_ip_link(IP_LINK *head)
{
    IP_LINK *current = head;
    IP_LINK *next_node;

    while (current != NULL) {
        next_node = current->next; // 先保存下一个节点的地址
        free(current);             // 释放当前节点
        current = next_node;       // 移动到下一个节点
    }
    // 释放后，将全局头指针置为NULL，避免野指针
    // 注意：这里需要传入全局指针的地址或者直接操作全局指针
    // 为了符合函数签名，我们假设调用者会在外部将ip_head = NULL;
    // 或者函数应该修改为 `void free_ip_link(IP_LINK **head_ptr)`
    // 由于头文件已固定为 `IP_LINK *ip_head`，这里就不修改了
    // 实际操作时，外部调用 free_ip_link(ip_head); ip_head = NULL;
    printf("IP filter list freed successfully.\n");
}

/******************************************************************
函  数: IP_LINK *del_ip_for_link(IP_LINK *head, unsigned char *ip)
功  能: 删除ip过滤链表节点
参  数: IP_LINK *head       - ip过滤链表头
       unsigned char *ip - 待删除IP
返回值: IP_LINK *head       - 更新后的链表头
*******************************************************************/
IP_LINK *del_ip_for_link(IP_LINK *head, unsigned char *ip)
{
    if (head == NULL || ip == NULL) {
        return head;
    }

    IP_LINK *current = head;
    IP_LINK *prev = NULL;

    // 如果要删除的是头节点
    if (memcmp(current->ip, ip, 4) == 0) {
        head = current->next;
        free(current);
        printf("Deleted IP %d.%d.%d.%d from list.\n", ip[0], ip[1], ip[2], ip[3]);
        return head;
    }

    // 查找并删除非头节点
    prev = current;
    current = current->next;
    while (current != NULL) {
        if (memcmp(current->ip, ip, 4) == 0) {
            prev->next = current->next;
            free(current);
            printf("Deleted IP %d.%d.%d.%d from list.\n", ip[0], ip[1], ip[2], ip[3]);
            return head;
        }
        prev = current;
        current = current->next;
    }

    printf("IP %d.%d.%d.%d not found in list for deletion.\n", ip[0], ip[1], ip[2], ip[3]);
    return head; // 未找到待删除IP，返回原链表头
}

/******************************************************************
函  数: void init_ip_link()
功  能: 读取配置文件数据到链表
参  数: 无
返回值: 无
*******************************************************************/
void init_ip_link()
{
    FILE *fp = NULL;
    unsigned int ip1, ip2, ip3, ip4;
    
    // 尝试打开文件进行读取
    fp = fopen(IP_FILTER_CONFIG_FILE, "r");
    if (fp == NULL) {
        fprintf(stderr, "Warning: Could not open config file '%s' for reading. Starting with empty list.\n", IP_FILTER_CONFIG_FILE);
        return; // 文件不存在或无法打开，链表保持为空
    }

    // 每次启动时，先清空现有链表（如果已经存在的话）
    if (ip_head != NULL) {
        free_ip_link(ip_head);
        ip_head = NULL; // 确保全局头指针置空
    }
    
    printf("Loading IP filter list from '%s'...\n", IP_FILTER_CONFIG_FILE);
    while (fscanf(fp, "%u.%u.%u.%u", &ip1, &ip2, &ip3, &ip4) == 4) {
        // 分配新节点内存
        IP_LINK *new_node = (IP_LINK *)malloc(sizeof(IP_LINK));
        if (new_node == NULL) {
            fprintf(stderr, "Error: Memory allocation failed for new IP node during init.\n");
            break; // 内存分配失败，停止读取
        }
        
        // 填充IP数据
        new_node->ip[0] = (unsigned char)ip1;
        new_node->ip[1] = (unsigned char)ip2;
        new_node->ip[2] = (unsigned char)ip3;
        new_node->ip[3] = (unsigned char)ip4;
        new_node->next = NULL; // inner_ip_link 会处理next指针

        // 插入到链表
        ip_head = inner_ip_link(ip_head, new_node);
    }

    fclose(fp);
    printf("IP filter list loaded.\n");
}

/******************************************************************
函  数: void save_ip_link()
功  能: 保存链表数据到配置文件
参  数: 无
返回值: 无
*******************************************************************/
void save_ip_link()
{
    FILE *fp = NULL;
    IP_LINK *current = ip_head;

    // 尝试打开文件进行写入 (w模式会清空现有内容)
    fp = fopen(IP_FILTER_CONFIG_FILE, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Could not open config file '%s' for writing.\n", IP_FILTER_CONFIG_FILE);
        return;
    }

    printf("Saving IP filter list to '%s'...\n", IP_FILTER_CONFIG_FILE);
    while (current != NULL) {
        fprintf(fp, "%u.%u.%u.%u\n",
                current->ip[0], current->ip[1], current->ip[2], current->ip[3]);
        current = current->next;
    }

    fclose(fp);
    printf("IP filter list saved.\n");
}
