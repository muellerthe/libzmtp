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
#include "net/gnrc/tcp.h"
#endif

struct zmtp_tcp_endpoint {
    zmtp_endpoint_t base;
#ifdef MODULE_GNRC_TCP
    ipv6_addr_t addr;       //active: target_addr; passive: local_addr
    uint16_t port;          //active: target_port; passive: local_port
#else
    struct addrinfo *addrinfo;
#endif
};


zmtp_tcp_endpoint_t *
zmtp_tcp_endpoint_new (const char *ip_addr, unsigned short port)
{
    zmtp_tcp_endpoint_t *self =
        (zmtp_tcp_endpoint_t *) zmalloc (sizeof *self);
    if (!self)
        return NULL;

    //  Initialize base class
    self->base = (zmtp_endpoint_t) {
#ifdef MODULE_GNRC_TCP
        .connect = (gnrc_tcp_tcb_t (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_connect,    
        .listen = (gnrc_tcp_tcb_t (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_listen,
#else
        .connect = (int (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_connect,    
        .listen = (int (*) (zmtp_endpoint_t *)) zmtp_tcp_endpoint_listen,
#endif
        .destroy = (void (*) (zmtp_endpoint_t **)) zmtp_tcp_endpoint_destroy,
    };

#ifdef MODULE_GNRC_TCP
    ipv6_addr_from_str(&self->addr, ip_addr);
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
        freeaddrinfo (self->addrinfo);          //ändern ?? 
#endif
        free (self);
        *self_p = NULL;
    }
}

#ifdef MODULE_GNRC_TCP
gnrc_tcp_tcb_t 
zmtp_tcp_endpoint_connect (zmtp_tcp_endpoint_t *self)
#else
int
zmtp_tcp_endpoint_connect (zmtp_tcp_endpoint_t *self)
#endif
{
    assert (self);

#ifdef MODULE_GNRC_TCP
    gnrc_tcp_tcb_t tcb;
    socket (&tcb);         //init tcb  

    const int rc = connect (&tcb, self->addr, self->port);      //open_active 

    if (rc != 0) {
        close (rc);  
        //return NULL;
    }

    return tcb;     
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
gnrc_tcp_tcb_t 
zmtp_tcp_endpoint_listen (zmtp_tcp_endpoint_t *self)
#else
int
zmtp_tcp_endpoint_listen (zmtp_tcp_endpoint_t *self)
#endif
{
    assert (self);
#ifdef MODULE_GNRC_TCP
    gnrc_tcp_tcb_t tcb;
    socket(&tcb);      // init tcb

    const int rc = bind_listen_accept(&tcb, self->addr, self->port);      
    
    if(rc != 0)
        close(rc);

    return tcb;

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
