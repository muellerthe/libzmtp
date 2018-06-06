#include "net/gnrc/tcp.h"


int close (gnrc_tcp_tcp_t *tcb) {
    gnrc_tcp_close(&tcb);	
    return 0;
}

ssize_t send(gnrc_tcp_tcp_t *tcb, const void *buf, size_t len, int flags) {
    ssize_t ret = gnrc_tcp_send(&tcb, buf, len, 0);
    return ret;
}

ssize_t recv(gnrc_tcp_tcp_t *tcb, void *buf, size_t len, int flags) {
    ssize_t ret = gnrc_tcp_recv(&tcb, buf, len, 0);
    return ret;
}