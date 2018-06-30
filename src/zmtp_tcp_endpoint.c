/*  =========================================================================
    zmtp_tcp_endpoint - TCP endpoint class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

#ifdef MODULE_GNRC_TCP
#include "zmtp_tcp_endpoint_wrapper.h"
#endif

struct zmtp_tcp_endpoint {
    zmtp_endpoint_t base;
#ifdef MODULE_GNRC_TCP
    char *addr;       
    uint16_t port;       
#else
    struct addrinfo *addrinfo;
#endif
};


zmtp_tcp_endpoint_t *
zmtp_tcp_endpoint_new (const char *ip_addr, unsigned short port)
{
    zmtp_tcp_endpoint_t *self =
        (zmtp_tcp_endpoint_t *) malloc (sizeof *self);
    if (!self)
        return NULL;

    //  Initialize base class
    self->base = (zmtp_endpoint_t) {
#ifdef MODULE_GNRC_TCP
        .connect = (int (*) (gnrc_tcp_tcb_t*, zmtp_endpoint_t *)) zmtp_tcp_endpoint_connect,    
        .listen = (int (*) (gnrc_tcp_tcb_t*, zmtp_endpoint_t *)) zmtp_tcp_endpoint_listen,
#else
        .connect = (int (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_connect,    
        .listen = (int (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_listen,
#endif
        .destroy = (void (*) (zmtp_endpoint_t **)) zmtp_tcp_endpoint_destroy,
    };


#ifdef MODULE_GNRC_TCP
    //  Fill in the rest of the struct
    self->addr = (char *) malloc(strlen(ip_addr) + 1);
    memcpy(self->addr, ip_addr, strlen(ip_addr) + 1);         
    self->port = (uint16_t) port;
#else
    //  Resolve address
    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_NUMERICHOST | AI_NUMERICSERV
    };
    char service [8 + 1];
    snprintf (service, sizeof service, "%u", port);
    if (getaddrinfo (ip_addr, service, &hints, &self->addrinfo)) {
        free (self);
        return NULL;
    }
#endif

    return self;
}


void
zmtp_tcp_endpoint_destroy (zmtp_tcp_endpoint_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_tcp_endpoint_t *self = *self_p;
#ifndef MODULE_GNRC_TCP       
        freeaddrinfo (self->addrinfo);        
#endif
        free (self);
        *self_p = NULL;
    }
}

#ifdef MODULE_GNRC_TCP
int 
zmtp_tcp_endpoint_connect (gnrc_tcp_tcb_t *tcb, zmtp_tcp_endpoint_t *self)
#else
int
zmtp_tcp_endpoint_connect (zmtp_tcp_endpoint_t *self)
#endif
{
    assert (self);

#ifdef MODULE_GNRC_TCP
    //  Initialize TCB and open a connection actively
    socket (tcb);        
    const int rc = connect (tcb, self->addr, self->port);   

    // For debugging: to make sure the tcb struct was initialized correctly
    // printf("\n\nDEBUG zmtp_tcp_endpoint_connect: tcb->local_addr: "); 
    // ipv6_addr_print((ipv6_addr_t *) tcb->local_addr);
    // printf("\nDEBUG zmtp_tcp_endpoint_connect: tcb->peer_addr: "); 
    // ipv6_addr_print((ipv6_addr_t *) tcb->peer_addr);
    // printf("\nDEBUG zmtp_tcp_endpoint_connect: tcb->ll_iface: %d\n", tcb->ll_iface); 
    // printf("DEBUG zmtp_tcp_endpoint_connect: tcb->local_port: %" PRIu16 "\n", tcb->local_port); 
    // printf("DEBUG zmtp_tcp_endpoint_connect: tcb->peer_port: %" PRIu16 "\n", tcb->peer_port); 
    // printf("DEBUG zmtp_tcp_endpoint_connect: tcb->state: %" PRIu8 "\n", tcb->state); 
    // printf("DEBUG zmtp_tcp_endpoint_connect: tcb->iss: %" PRIu32 "\n", tcb->iss); 
    // printf("DEBUG zmtp_tcp_endpoint_connect: tcb->irs: %" PRIu32 "\n\n", tcb->irs); 

    return rc; 

#else
    const int s = socket (AF_INET, SOCK_STREAM, 0);
    if (s == -1)            
        return -1;

    const int rc = connect (
        s, self->addrinfo->ai_addr, self->addrinfo->ai_addrlen); 

    if (rc == -1) {
        close (s);      
        return -1;
    }

    return s; 

#endif
}

#ifdef MODULE_GNRC_TCP
int 
zmtp_tcp_endpoint_listen (gnrc_tcp_tcb_t *tcb, zmtp_tcp_endpoint_t *self)
#else
int
zmtp_tcp_endpoint_listen (zmtp_tcp_endpoint_t *self)
#endif
{
    assert (self);

#ifdef MODULE_GNRC_TCP
    socket (tcb);
    const int rc = bind_listen_accept(tcb, self->port);   
    return rc;

#else
    const int s = socket (AF_INET, SOCK_STREAM, 0);
    if (s == -1)
        return -1;

    const int flag = 1;
    int rc = setsockopt (s, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof flag);
    assert (rc == 0);

    rc = bind (
        s, self->addrinfo->ai_addr, self->addrinfo->ai_addrlen);
    if (rc == 0) {
        rc = listen (s, 1);
        if (rc == 0)
            rc = accept (s, NULL, NULL);
    }
    close (s);
    return rc;
#endif
}
