/*
 * netsniff-ng - the packet sniffing beast
 * Copyright 2012 Daniel Borkmann <borkmann@iogearbox.net>
 * Copyright 2012 Markus Amend <markus@netsniff-ng.org>, Deutsche Flugsicherung GmbH
 * Subject to the GPL, version 2.
 */

#include <stdio.h>
#include <stdint.h>
#include <netinet/in.h>    /* for ntohs() */
#include <asm/byteorder.h>

#include "proto.h"
#include "protos.h"
#include "dissector_80211.h"
#include "built_in.h"
#include "pkt_buff.h"
#include "oui.h"

/* Note: Fields are encoded in little-endian! */
struct ieee80211_frm_ctrl {
	union {
		u16 frame_control;
		struct {
#if defined(__LITTLE_ENDIAN_BITFIELD)
		/* Correct order here ... */
		__extension__ u16 proto_version:2,
				  type:2,
				  subtype:4,
				  to_ds:1,
				  from_ds:1,
				  more_frags:1,
				  retry:1,
				  power_mgmt:1,
				  more_data:1,
				  wep:1,
				  order:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
		__extension__ u16 subtype:4,
				  type:2,
				  proto_version:2,
				  order:1,
				  wep:1,
				  more_data:1,
				  power_mgmt:1,
				  retry:1,
				  more_frags:1,
				  from_ds:1,
				  to_ds:1;
#else
# error  "Adjust your <asm/byteorder.h> defines"
#endif
		};
	};
} __packed;

/* Management Frame start */
/* Note: Fields are encoded in little-endian! */
struct ieee80211_mgmt {
	u16 duration;
	u8 da[6];
	u8 sa[6];
	u8 bssid[6];
	u16 seq_ctrl;
} __packed;

struct ieee80211_mgmt_auth {
	u16 auth_alg;
	u16 auth_transaction;
	u16 status_code;
	/* possibly followed by Challenge text */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_deauth {
	u16 reason_code;
} __packed;

struct ieee80211_mgmt_assoc_req {
	u16 capab_info;
	u16 listen_interval;
	/* followed by SSID and Supported rates */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_assoc_resp {
	u16 capab_info;
	u16 status_code;
	u16 aid;
	/* followed by Supported rates */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_reassoc_resp {
	u16 capab_info;
	u16 status_code;
	u16 aid;
	/* followed by Supported rates */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_reassoc_req {
	u16 capab_info;
	u16 listen_interval;
	u8 current_ap[6];
	/* followed by SSID and Supported rates */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_disassoc {
	u16 reason_code;
} __packed;

struct ieee80211_mgmt_probe_req {
} __packed;

struct ieee80211_mgmt_beacon {
	u8 timestamp[8];
	u16 beacon_int;
	u16 capab_info;
	/* followed by some of SSID, Supported rates,
	  * FH Params, DS Params, CF Params, IBSS Params, TIM */
	u8 variable[0];
} __packed;

struct ieee80211_mgmt_probe_resp {
	u8 timestamp[8];
	u16 beacon_int;
	u16 capab_info;
	/* followed by some of SSID, Supported rates,
	  * FH Params, DS Params, CF Params, IBSS Params, TIM */
	u8 variable[0];
} __packed;
/* Management Frame end */

/* Control Frame start */
/* Note: Fields are encoded in little-endian! */
struct ieee80211_ctrl {
} __packed;

struct ieee80211_ctrl_rts {
	u16 duration;
	u8 da[6];
	u8 sa[6];	
} __packed;

struct ieee80211_ctrl_cts {
	u16 duration;
	u8 da[6];
} __packed;

struct ieee80211_ctrl_ack {
	u16 duration;
	u8 da[6];
} __packed;

struct ieee80211_ctrl_ps_poll {
	u16 aid;
	u8 bssid[6];
	u8 sa[6];
} __packed;

struct ieee80211_ctrl_cf_end {
	u16 duration;
	u8 bssid[6];
	u8 sa[6];
} __packed;

struct ieee80211_ctrl_cf_end_ack {
	u16 duration;
	u8 bssid[6];
	u8 sa[6];
} __packed;
/* Control Frame end */

/* Data Frame start */
/* Note: Fields are encoded in little-endian! */
struct ieee80211_data {
} __packed;

/* TODO: Extend */
/* Control Frame end */


/* http://www.sss-mag.com/pdf/802_11tut.pdf
 * http://www.scribd.com/doc/78443651/111/Management-Frames
 * http://www.wildpackets.com/resources/compendium/wireless_lan/wlan_packets
 * http://www.rhyshaden.com/wireless.htm
*/

static int8_t assoc_req(struct pkt_buff *pkt) {
	return 0;
}

static int8_t assoc_resp(struct pkt_buff *pkt) {
	return 0;
}

static char *mgt_sub(u8 subtype, int8_t (**get_content)(struct pkt_buff *pkt)) {
	switch (subtype) {
	case 0b0000:
		      *get_content = assoc_req;
		      return "Association Request";
	case 0b0001:
		      *get_content = assoc_resp;
		      return "Association Response";
	}

	return "Management SubType not supported";
}

static char *ctrl_sub(u8 subtype, int8_t (**get_content)(struct pkt_buff *pkt)) {
	return "Control SubType not supported";
}

static char *data_sub(u8 subtype, int8_t (**get_content)(struct pkt_buff *pkt)) {
	return "Data SubType not supported";
}

static char *frame_control_type(u8 type, char *(**get_subtype)(u8 subtype, int8_t (**get_content)(struct pkt_buff *pkt))) {
	switch (type) {
	case 0b00:
		    *get_subtype = mgt_sub;
		    return "Management";
	case 0b01:
		    *get_subtype = ctrl_sub;
		    return "Control";
	case 0b10:
		    *get_subtype = data_sub;
		    return "Data";
	case 0b11: return "Reserved";
	}

	return "Control Type not supported";
	
}

static void ieee80211(struct pkt_buff *pkt)
{
	int8_t (*get_content)(struct pkt_buff *pkt) = NULL;
	char *(*get_subtype)(u8 subtype, int8_t (**get_content)(struct pkt_buff *pkt)) = NULL;
	
	struct ieee80211_frm_ctrl *frm_ctrl =
		(struct ieee80211_frm_ctrl *) pkt_pull(pkt, sizeof(*frm_ctrl));
	if (frm_ctrl == NULL)
		return;

	tprintf(" [ 802.11 Frame Control (0x%04x)]\n",
		le16_to_cpu(frm_ctrl->frame_control));
	tprintf("\t [ Proto Version (%u), ", frm_ctrl->proto_version);
	tprintf("Type (%u, %s), ", frm_ctrl->type, frame_control_type(frm_ctrl->type, &get_subtype));
	if (get_subtype)
		tprintf("Subtype (%u, %s)", frm_ctrl->subtype, (*get_subtype)(frm_ctrl->subtype, &get_content));
	else
		tprintf("\n%s%s%s", colorize_start_full(black, red),
			    "Failed to get Subtype", colorize_end());
	tprintf("%s%s",
		frm_ctrl->to_ds ? ", Frame goes to DS" : "",
		frm_ctrl->from_ds ?  ", Frame comes from DS" : "");
	tprintf("%s", frm_ctrl->more_frags ? ", More Fragments" : "");
	tprintf("%s", frm_ctrl->retry ? ", Frame is retransmitted" : "");
	tprintf("%s", frm_ctrl->power_mgmt ? ", In Power Saving Mode" : "");
	tprintf("%s", frm_ctrl->more_data ? ", More Data" : "");
	tprintf("%s", frm_ctrl->wep ? ", Needs WEP" : "");
	tprintf("%s", frm_ctrl->order ? ", Order" : "");
	tprintf(" ]\n");

	if (get_content) {
		if (!((*get_content) (pkt)))
		      tprintf("\n%s%s%s", colorize_start_full(black, red),
			    "Failed to dissect Subtype", colorize_end());
	}
	else
		tprintf("\n%s%s%s", colorize_start_full(black, red),
			    "Failed to dissect Subtype", colorize_end());

//	pkt_set_proto(pkt, &ieee802_lay2, ntohs(eth->h_proto));
}

static void ieee80211_less(struct pkt_buff *pkt)
{
	tprintf("802.11 frame (more on todo)");
}

struct protocol ieee80211_ops = {
	.key = 0,
	.print_full = ieee80211,
	.print_less = ieee80211_less,
};

EXPORT_SYMBOL(ieee80211_ops);
