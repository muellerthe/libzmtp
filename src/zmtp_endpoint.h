/*  =========================================================================
    zmtp_endpoint - endpoint base class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef __ZMTP_ENDPOINT_H_INCLUDED__
#define __ZMTP_ENDPOINT_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_GNRC_TCP
#include "net/gnrc/tcp.h"
#endif

struct zmtp_endpoint {
    void (*destroy) (struct zmtp_endpoint **self_p);

#ifdef MODULE_GNRC_TCP
    int (*connect) (gnrc_tcp_tcb_t *tcb, struct zmtp_endpoint *self);
    int (*listen) (gnrc_tcp_tcb_t *tcb, struct zmtp_endpoint *self);
#else
    int (*connect) (struct zmtp_endpoint *self);            
    int (*listen) (struct zmtp_endpoint *self);             
#endif
};

typedef struct zmtp_endpoint zmtp_endpoint_t;

void
    zmtp_endpoint_destroy (zmtp_endpoint_t **self_p);

#ifdef MODULE_GNRC_TCP
int 
    zmtp_endpoint_connect (gnrc_tcp_tcb_t *tcb, zmtp_endpoint_t *self);
int 
    zmtp_endpoint_listen (gnrc_tcp_tcb_t *tcb, zmtp_endpoint_t *self);
#else
int
    zmtp_endpoint_connect (zmtp_endpoint_t *self);
int
    zmtp_endpoint_listen (zmtp_endpoint_t *self);
#endif

#endif
