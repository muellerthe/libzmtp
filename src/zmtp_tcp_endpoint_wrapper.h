#include "net/af.h"
#include "net/gnrc/ipv6.h"
#include "net/ipv6/addr.h"
#include "net/gnrc/tcp.h"

void socket(gnrc_tcp_tcb_t *tcb);

int connect (gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port);

int bind_listen_accept(gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port);