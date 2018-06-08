int close_connection(gnrc_tcp_tcb_t *tcb);

ssize_t send(gnrc_tcp_tcb_t *tcb, const void *buf, size_t len, int flags);

ssize_t recv(gnrc_tcp_tcb_t *tcb, void *buf, size_t len, int flags);
