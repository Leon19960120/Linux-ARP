#ifndef KEY_THREAD_H
#define KEY_THREAD_H

void *key_thread(void *arg);

// 如果 arp_head 和 ip_head 是由 key_thread.c 定义的全局变量
// 并且其他文件需要访问，那么它们需要在这里声明为 extern。
// 但更常见的情况是，它们由 arp_link.c 和 ip_link.c 定义，并在其对应的头文件中声明。
// 例如：
// extern struct arp_entry *arp_head; // 假设 arp_entry 是 ARP 表的节点类型
// extern struct ip_filter_rule *ip_head; // 假设 ip_filter_rule 是 IP 过滤规则的节点类型

// 如果 printf_arp_link 和 printf_ip_link 在其他文件定义，那么 key_thread.c 会包含那些文件的头文件。
// 它们不需要在 key_thread.h 中声明。
// 例如，如果它们分别在 arp_link.h 和 ip_link.h 中声明：
// #include "arp_link.h"
// #include "ip_link.h"

#endif /* KEY_THREAD_H */