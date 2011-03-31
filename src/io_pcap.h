/*
 * netsniff-ng - the packet sniffing beast
 * By Daniel Borkmann <daniel@netsniff-ng.org>
 * Copyright 2009, 2010 Daniel Borkmann.
 * Copyright 2010 Emmanuel Roullit.
 * Subject to the GPL.
 */

#ifndef IO_PCAP_H
#define IO_PCAP_H

#include <sys/time.h>
#include <linux/if_packet.h>

#include "compiler.h"

#define TCPDUMP_MAGIC              0xa1b2c3d4
#define PCAP_VERSION_MAJOR         2
#define PCAP_VERSION_MINOR         4
#define PCAP_DEFAULT_SNAPSHOT_LEN  65535

#define LINKTYPE_NULL              0   /* BSD loopback encapsulation */
#define LINKTYPE_EN10MB            1   /* Ethernet (10Mb) */
#define LINKTYPE_EN3MB             2   /* Experimental Ethernet (3Mb) */
#define LINKTYPE_AX25              3   /* Amateur Radio AX.25 */
#define LINKTYPE_PRONET            4   /* Proteon ProNET Token Ring */
#define LINKTYPE_CHAOS             5   /* Chaos */
#define LINKTYPE_IEEE802           6   /* 802.5 Token Ring */
#define LINKTYPE_ARCNET            7   /* ARCNET, with BSD-style header */
#define LINKTYPE_SLIP              8   /* Serial Line IP */
#define LINKTYPE_PPP               9   /* Point-to-point Protocol */
#define LINKTYPE_FDDI              10  /* FDDI */

struct pcap_filehdr {
	uint32_t magic;
	uint16_t version_major;
	uint16_t version_minor;
	int32_t thiszone;
	uint32_t sigfigs;
	uint32_t snaplen;
	uint32_t linktype;
};

struct pcap_timeval {
	int32_t tv_sec;
	int32_t tv_usec;
};

struct pcap_nsf_pkthdr {
	struct timeval ts;
	uint32_t caplen;
	uint32_t len;
};

struct pcap_pkthdr {
	struct pcap_timeval ts;
	/*
	 * If netsniff-ngs zLib compression used, this is the buff len
	 * for inflate.
	 */
	uint32_t caplen;
	uint32_t len;
};

extern __must_check int pcap_write_file_header(int fd, uint32_t linktype,
					       int32_t thiszone,
					       uint32_t snaplen);
extern __must_check ssize_t pcap_write_pkt(int fd, struct pcap_pkthdr *hdr,
					   uint8_t *packet);
extern __must_check int pcap_read_and_validate_file_header(int fd);
extern __must_check int pcap_read_still_has_packets(int fd);
extern __must_check ssize_t pcap_read_packet(int fd, struct pcap_pkthdr *hdr,
					     uint8_t *packet, size_t len);

static inline void tpacket_hdr_to_pcap_pkthdr(struct tpacket_hdr *thdr,
					      struct pcap_pkthdr *phdr)
{
	phdr->ts.tv_sec = thdr->tp_sec;
	phdr->ts.tv_usec = thdr->tp_usec;
	phdr->caplen = thdr->tp_snaplen;
	phdr->len = thdr->tp_len;
}

static inline void pcap_pkthdr_to_tpacket_hdr(struct pcap_pkthdr *phdr,
					      struct tpacket_hdr *thdr)
{
	thdr->tp_sec = phdr->ts.tv_sec;
	thdr->tp_usec = phdr->ts.tv_usec;
	thdr->tp_snaplen = phdr->caplen;
	thdr->tp_len = phdr->len;
}

#endif /* IO_PCAP_H */