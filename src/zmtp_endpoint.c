/*  =========================================================================
    zmtp_endpoint - endpoint base class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

//  --------------------------------------------------------------------------
//  Destructor

void
zmtp_endpoint_destroy (zmtp_endpoint_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_endpoint_t *self = *self_p;
        assert (self->destroy);
        self->destroy (self_p);
    }
}


//  --------------------------------------------------------------------------
//  Connect to the endpoint

#ifdef MODULE_GNRC_TCP
int 
zmtp_endpoint_connect (gnrc_tcp_tcb_t *tcb, zmtp_endpoint_t *self)
#else
int
zmtp_endpoint_connect (zmtp_endpoint_t *self)
#endif
{
    assert (self);
    assert (self->connect);

#ifdef MODULE_GNRC_TCP
    return self->connect (tcb, self);
#else
    return self->connect (self);
#endif
}


//  --------------------------------------------------------------------------
//  Listen for new connection on endpoint

#ifdef MODULE_GNRC_TCP
int 
zmtp_endpoint_listen (gnrc_tcp_tcb_t *tcb, zmtp_endpoint_t *self)
#else
int
zmtp_endpoint_listen (zmtp_endpoint_t *self)
#endif
{
    assert (self);
    assert (self->listen);

#ifdef MODULE_GNRC_TCP
    return self->listen (tcb, self);
#else
    return self->listen (self);
#endif
}
