#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "key_thread.h"
#include "arp_link.h"
#include "ip_link.h"
#include "main.h"

static int parse_ip4(const char *s, unsigned char ip[4])
{
    unsigned int a, b, c, d;
    if (sscanf(s, "%u.%u.%u.%u", &a, &b, &c, &d) != 4) {
        return -1;
    }
    if (a > 255 || b > 255 || c > 255 || d > 255) {
        return -1;
    }
    ip[0] = (unsigned char)a;
    ip[1] = (unsigned char)b;
    ip[2] = (unsigned char)c;
    ip[3] = (unsigned char)d;
    return 0;
}

void *key_thread(void *arg)
{
    (void)arg;

    char cmd[64];
    while (fgets(cmd, sizeof(cmd), stdin) != NULL) {
        if (strncmp(cmd, "arp", 3) == 0) {
            printf_arp_link(arp_head);
        } else if (strncmp(cmd, "ip", 2) == 0) {
            printf_ip_link(ip_head);
        } else if (strncmp(cmd, "setip ", 6) == 0) {
            unsigned char ip[4];
            if (parse_ip4(cmd + 6, ip) != 0) {
                printf("Usage: setip a.b.c.d\n");
                continue;
            }
            if (find_ip(ip_head, ip) != NULL) {
                printf("IP already exists in blacklist.\n");
                continue;
            }
            IP_LINK *node = (IP_LINK *)malloc(sizeof(IP_LINK));
            if (node == NULL) {
                perror("malloc");
                continue;
            }
            memcpy(node->ip, ip, 4);
            node->next = NULL;
            ip_head = inner_ip_link(ip_head, node);
            save_ip_link();
            printf("Added blacklist IP: %u.%u.%u.%u\n", ip[0], ip[1], ip[2], ip[3]);
        } else if (strncmp(cmd, "delip ", 6) == 0) {
            unsigned char ip[4];
            if (parse_ip4(cmd + 6, ip) != 0) {
                printf("Usage: delip a.b.c.d\n");
                continue;
            }
            ip_head = del_ip_for_link(ip_head, ip);
            save_ip_link();
        } else if (strncmp(cmd, "quit", 4) == 0) {
            g_running = 0;
            close(raw_sock_fd);
            break;
        } else if (strncmp(cmd, "help", 4) == 0) {
            printf("Commands: ip | arp | setip a.b.c.d | delip a.b.c.d | quit | help\n");
        }
    }

    return NULL;
}
