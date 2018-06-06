int close(gnrc_tcp_tcp_t *tcb);

ssize_t send(gnrc_tcp_tcp_t *tcb, const void *buf, size_t len, int flags);

ssize_t recv(gnrc_tcp_tcp_t *tcb, void *buf, size_t len, int flags);