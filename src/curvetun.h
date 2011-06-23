/*
 * netsniff-ng - the packet sniffing beast
 * By Daniel Borkmann <daniel@netsniff-ng.org>
 * Copyright 2011 Daniel Borkmann.
 * Subject to the GPL.
 */

#ifndef CURVETUN_H
#define CURVETUN_H

#define DEFAULT_CURVE   "secp521r1/nistp521"
#define FILE_CLIENTS    ".curvetun/clients"
#define FILE_SERVERS    ".curvetun/servers"
#define FILE_PRIVKEY    ".curvetun/priv.key"
#define FILE_PUBKEY     ".curvetun/pub.key"
#define FILE_USERNAM    ".curvetun/username"

#define DEFAULT_KEY_LEN 64

#define EXIT_SEQ        "\r\r\r"

#define PROTO_FLAG_EXIT	(1 << 0)

struct ct_proto {
        uint16_t payload;
        uint8_t flags;
}  __attribute__((packed));

#endif /* CURVETUN_H */
