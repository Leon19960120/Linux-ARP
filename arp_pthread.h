#ifndef ARP_PTHREAD_H
#define ARP_PTHREAD_H

#include <sys/types.h>

void handle_arp_packet(char *buf, ssize_t len);
void *arp_pthread(void *arg);

#endif