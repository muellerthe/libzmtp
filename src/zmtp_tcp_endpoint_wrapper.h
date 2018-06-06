void socket(gnrc_tcp_tcb_t *tcb);

int connect (gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port);

int bind_listen_accept(gnrc_tcp_tcb_t *tcb, ipv6_addr_t addr, uint16_t port);