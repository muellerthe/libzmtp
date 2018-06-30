#define ENABLE_DEBUG (0)
#include "debug.h"

#include "zmtp_tcp_endpoint_wrapper.h"


void socket(gnrc_tcp_tcb_t *tcb) 
{
    gnrc_tcp_tcb_init(tcb);	
}

int connect (gnrc_tcp_tcb_t *tcb, char *target_addr, uint16_t target_port) 
{
    int ret = gnrc_tcp_open_active(tcb, AF_INET6, target_addr, target_port, 0);
    switch (ret) {
        case 0:
            DEBUG("gnrc_tcp_open_active() : 0 : ok\n");
            break;

        case -EISCONN:
            printf("gnrc_tcp_open_active() : -EISCONN\n");
            return 1;

        case -EINVAL:
            printf("gnrc_tcp_open_active() : -EINVAL\n");
            return 1;

        case -EAFNOSUPPORT:
            printf("gnrc_tcp_open_active() : -EAFNOSUPPORT\n");
            return 1;

        case -EADDRINUSE:
            printf("gnrc_tcp_open_active() : -EADDRINUSE\n");
            return 1;

        case -ECONNREFUSED:
            printf("gnrc_tcp_open_active() : -ECONNREFUSED\n");
            return 1;

        case -ENOMEM:
            printf("gnrc_tcp_open_active() : -ENOMEM\n");
            return 1;

        case -ETIMEDOUT:
            printf("gnrc_tcp_open_active() : -ETIMEDOUT\n");
            return 1;

        default:
            printf("gnrc_tcp_open_active() : %d\n", ret);
            return 1;
    }

    return ret;
}

int bind_listen_accept(gnrc_tcp_tcb_t *tcb, uint16_t local_port) 
{
    int ret = gnrc_tcp_open_passive(tcb, AF_INET6, NULL, local_port);
    switch (ret) {
        case 0:
            DEBUG("gnrc_tcp_open_passive() : 0 : ok\n");
            break;

        case -EISCONN:
            printf("gnrc_tcp_open_passive() : -EISCONN\n");
            return 1;

        case -EINVAL:
            printf("gnrc_tcp_open_passive() : -EINVAL\n");
            return 1;

        case -EAFNOSUPPORT:
            printf("gnrc_tcp_open_passive() : -EAFNOSUPPORT\n");
            return 1;

        case -ENOMEM:
            printf("gnrc_tcp_open_passive() : -ENOMEM\n");
            return 1;

        default:
            printf("gnrc_tcp_open_passive() : %d\n", ret);
            return 1;
    }

    return ret;
}
