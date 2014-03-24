/*  =========================================================================
    zmtp_channel - channel class

    Copyright (c) contributors as noted in the AUTHORS file.
    This file is part of libzmtp, the C ZMTP stack.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "zmtp_classes.h"

//  ZMTP greeting (64 bytes)

struct zmtp_greeting {
    byte signature [10];
    byte version [2];
    byte mechanism [20];
    byte as_server [1];
    byte filler [31];
};

//  Structure of our class

struct _zmtp_channel_t {
    int fd;             //  BSD socket handle
};

static int
    s_negotiate (zmtp_channel_t *self);
static void
    s_tcp_send (int fd, const void *data, size_t len);
static void
    s_tcp_recv (int fd, void *buffer, size_t len);


//  --------------------------------------------------------------------------
//  Constructor

zmtp_channel_t *
zmtp_channel_new ()
{
    zmtp_channel_t *self = (zmtp_channel_t *) zmalloc (sizeof *self);
    assert (self);              //  For now, memory exhaustion is fatal
    self->fd = -1;
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor; closes fd if connected

void
zmtp_channel_destroy (zmtp_channel_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zmtp_channel_t *self = *self_p;
        if (self->fd != -1)
            close (self->fd);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Connect channel to local endpoint

int
zmtp_channel_ipc_connect (zmtp_channel_t *self, const char *path)
{
    assert (self);

    if (self->fd != -1)
        return -1;
    struct sockaddr_un remote = { .sun_family = AF_UNIX };
    if (strlen (path) >= sizeof remote.sun_path)
        return -1;
    strcpy (remote.sun_path, path);
    //  Create socket
    const int s = socket (AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
        return -1;
    //  Connect the socket
    const int rc =
        connect (s, (const struct sockaddr *) &remote, sizeof remote);
    if (rc == -1 || s_negotiate (self) == -1) {
        close (s);
        return -1;
    }
    self->fd = s;
    return 0;
}


//  --------------------------------------------------------------------------
//  Connect channel to TCP endpoint

int
zmtp_channel_tcp_connect (zmtp_channel_t *self,
                          const char *addr, unsigned short port)
{
    assert (self);

    if (self->fd != -1)
        return -1;
    //  Create socket
    const int s = socket (AF_INET, SOCK_STREAM, 0);
    if (s == -1)
        return -1;
    //  Resolve address
    const struct addrinfo hints = {
        .ai_family   = AF_INET,
        .ai_socktype = SOCK_STREAM,
        .ai_flags    = AI_NUMERICHOST | AI_NUMERICSERV
    };
    char service [8 + 1];
    snprintf (service, sizeof service, "%u", port);
    struct addrinfo *result = NULL;
    if (getaddrinfo (addr, service, &hints, &result)) {
        close (s);
        return -1;
    }
    assert (result);
    //  Create socket
    const int rc = connect (s, result->ai_addr, result->ai_addrlen);
    freeaddrinfo (result);
    if (rc == -1 || s_negotiate (self) == -1) {
        close (s);
        return -1;
    }
    self->fd = s;
    return 0;
}


//  --------------------------------------------------------------------------
//  Negotiate a ZMTP channel
//  This currently does only ZMTP v3, and will reject older protocols.
//  TODO: test sending random/wrong data to this handler.

static int
s_negotiate (zmtp_channel_t *self)
{
    //  This is our greeting (64 octets)
    const struct zmtp_greeting outgoing = {
        .signature = { 0xff, 0, 0, 0, 0, 0, 0, 0, 1, 0x7f },
        .version   = { 3, 0 },
        .mechanism = { 'N', 'U', 'L', 'L', '\0' }
    };
    //  Send protocol signature
    s_tcp_send (self->fd, outgoing.signature, sizeof outgoing.signature);

    //  Read the first byte.
    struct zmtp_greeting incoming;
    s_tcp_recv (self->fd, incoming.signature, 1);
    assert (incoming.signature [0] == 0xff);

    //  Read the rest of signature
    s_tcp_recv (self->fd, incoming.signature + 1, 9);
    assert ((incoming.signature [9] & 1) == 1);

    //  Exchange major version numbers
    s_tcp_send (self->fd, outgoing.version, 1);
    s_tcp_recv (self->fd, incoming.version, 1);

    assert (incoming.version [0] == 3);

    //  Send the rest of greeting to the peer.
    s_tcp_send (self->fd, outgoing.version + 1, 1);
    s_tcp_send (self->fd, outgoing.mechanism, sizeof outgoing.mechanism);
    s_tcp_send (self->fd, outgoing.as_server, sizeof outgoing.as_server);
    s_tcp_send (self->fd, outgoing.filler, sizeof outgoing.filler);

    //  Receive the rest of greeting from the peer.
    s_tcp_recv (self->fd, incoming.version + 1, 1);
    s_tcp_recv (self->fd, incoming.mechanism, sizeof incoming.mechanism);
    s_tcp_recv (self->fd, incoming.as_server, sizeof incoming.as_server);
    s_tcp_recv (self->fd, incoming.filler, sizeof incoming.filler);

    //  Send READY command
    zmtp_msg_t *ready = zmtp_msg_new_const (0x04, "READY   ", 8);
    zmtp_channel_send (self, ready);
    zmtp_msg_destroy (&ready);

    //  Receive READY command
    ready = zmtp_channel_recv (self);
    assert ((zmtp_msg_flags (ready) & 0x04) == 0x04);
    zmtp_msg_destroy (&ready);

    return 0;
}


//  --------------------------------------------------------------------------
//  Send a ZMTP message to the channel

int
zmtp_channel_send (zmtp_channel_t *self, zmtp_msg_t *msg)
{
    assert (self);
    assert (msg);

    byte frame_flags = zmtp_msg_flags (msg) & 0x04;
    if (zmtp_msg_size (msg) > 255)
        frame_flags |= 0x02;
    s_tcp_send (self->fd, &frame_flags, sizeof frame_flags);

    if (zmtp_msg_size (msg) <= 255) {
        const byte msg_size = zmtp_msg_size (msg);
        s_tcp_send (self->fd, &msg_size, sizeof msg_size);
    }
    else {
        byte buffer [8];
        const uint64_t msg_size = (uint64_t) zmtp_msg_size (msg);
        buffer [0] = msg_size >> 56;
        buffer [1] = msg_size >> 48;
        buffer [2] = msg_size >> 40;
        buffer [3] = msg_size >> 32;
        buffer [4] = msg_size >> 24;
        buffer [5] = msg_size >> 16;
        buffer [6] = msg_size >> 8;
        buffer [7] = msg_size;
        s_tcp_send (self->fd, buffer, sizeof buffer);
    }
    s_tcp_send (self->fd, zmtp_msg_data (msg), zmtp_msg_size (msg));
    return 0;
}


//  --------------------------------------------------------------------------
//  Receive a ZMTP message off the channel

zmtp_msg_t *
zmtp_channel_recv (zmtp_channel_t *self)
{
    assert (self);

    byte frame_flags;
    size_t size;

    s_tcp_recv (self->fd, &frame_flags, 1);
    //  Check large flag
    if ((frame_flags & 0x02) == 0) {
        byte buffer [1];
        s_tcp_recv (self->fd, buffer, 1);
        size = (size_t) buffer [0];
    }
    else {
        byte buffer [8];
        s_tcp_recv (self->fd, buffer, sizeof buffer);
        size = (uint64_t) buffer [7] << 56 |
               (uint64_t) buffer [6] << 48 |
               (uint64_t) buffer [5] << 40 |
               (uint64_t) buffer [4] << 32 |
               (uint64_t) buffer [3] << 24 |
               (uint64_t) buffer [2] << 16 |
               (uint64_t) buffer [1] << 8  |
               (uint64_t) buffer [0];
    }
    byte *data = zmalloc (size);
    assert (data);
    s_tcp_recv (self->fd, data, size);
    return zmtp_msg_new (frame_flags & 0x04, &data, size);
}


//  --------------------------------------------------------------------------
//  Lower-level TCP and ZMTP message I/O functions

static void
s_tcp_send (int fd, const void *data, size_t len)
{
    const ssize_t rc = send (fd, data, len, 0);
    assert (rc == len);
    assert (rc != -1);
}

static void
s_tcp_recv (int fd, void *buffer, size_t len)
{
    size_t bytes_read = 0;
    while (bytes_read < len) {
        const ssize_t n = read (
            fd, (char *) buffer + bytes_read, len - bytes_read);
        assert (n != 0);
        assert (n != -1);
        bytes_read += n;
    }
}


//  --------------------------------------------------------------------------
//  Selftest

void
zmtp_channel_test (bool verbose)
{
    printf (" * zmtp_channel: ");
    //  @selftest
    //  @end
    printf ("OK\n");
}
