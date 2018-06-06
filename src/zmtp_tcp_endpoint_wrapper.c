#include "net/gnrc/tcp.h"


void socket(gnrc_tcp_tcb_t *tcb) {
    gnrc_tcp_tcb_init(&tcb);	
}

int connect (gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port) {
    int ret = gnrc_tcp_open_active (&tcb, AF_INET6, (uint8_t *) &addr, port, 0); 
    return ret;
}

int bind_listen_accept(gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port) {
    int ret = gnrc_tcp_open_passive(&tcb, AF_INET6, NULL, port);
    return ret;
}