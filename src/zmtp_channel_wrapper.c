#define ENABLE_DEBUG (0)
#include "debug.h"

#include "zmtp_channel_wrapper.h"


int close_connection (gnrc_tcp_tcb_t *tcb) 
{
    gnrc_tcp_close(tcb);	
    return 0;
}

ssize_t send(gnrc_tcp_tcb_t *tcb, const void *buf, size_t len) 
{
    int ret = gnrc_tcp_send(tcb, buf, len, 0);

    switch (ret) {
        case -ENOTCONN:
            printf("gnrc_tcp_send() : -ENOTCONN\n");
            return -1;

        case -ECONNABORTED:
            printf("gnrc_tcp_send() : -ECONNABORTED\n");
            return -1;

        case -ETIMEDOUT:
            printf("gnrc_tcp_send() : -ETIMEDOUT\n");
            return -1;

        case -ECONNRESET:
            printf("gnrc_tcp_send() : -ECONNRESET\n");
            return -1;

        default:
            if (ret >= 0) {
                DEBUG("gnrc_tcp_send() : %d Bytes sent\n", ret);
            }
            else {
                printf("gnrc_tcp_send() : %d\n", ret);
                return -1;
            }
    }      

    return ret;
}

ssize_t recv(gnrc_tcp_tcb_t *tcb, void *buf, size_t len) 
{
    int ret = gnrc_tcp_recv(tcb, buf, len, GNRC_TCP_CONNECTION_TIMEOUT_DURATION);

    switch (ret) {
        case -ENOTCONN:
            printf("gnrc_tcp_rcvd() : -ENOTCONN\n");
            return -1;

        case -EAGAIN:
            printf("gnrc_tcp_rcvd() : -EAGAIN\n");
            return -1;

        case -ECONNABORTED:
            printf("gnrc_tcp_rcvd() : -ECONNABORTED\n");
            return -1;

        case -ECONNRESET:
            printf("gnrc_tcp_rcvd() : -ECONNRESET\n");
            return -1;

        case -ETIMEDOUT:
            printf("gnrc_tcp_rcvd() : -ETIMEDOUT\n");
            return -1;

        default:
            if (ret >= 0) {
                DEBUG("gnrc_tcp_rcvd() : %d Bytes read\n", ret);
            }
            else {
                printf("gnrc_tcp_rcvd() : %d\n", ret);
                return -1;
            }
    } 
    
    return ret;
}
