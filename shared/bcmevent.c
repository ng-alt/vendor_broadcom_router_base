/*
 * bcmevent read-only data shared by kernel or app layers
 *
 * Copyright (C) 2014, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * $Id: bcmevent.c 449760 2014-01-18 05:23:09Z $
 */

#include <typedefs.h>
#include <bcmutils.h>
#include <proto/ethernet.h>
#include <proto/bcmeth.h>
#include <proto/bcmevent.h>

/* Use the actual name for event tracing */
#define BCMEVENT_NAME(_event) {(_event), #_event}

/* Table of event name strings for UIs and debugging dumps */
typedef struct {
	uint event;
	const char *name;
} bcmevent_name_str_t;

static const bcmevent_name_str_t bcmevent_names[] = {
	BCMEVENT_NAME(WLC_E_SET_SSID),
	BCMEVENT_NAME(WLC_E_JOIN),
	BCMEVENT_NAME(WLC_E_START),
	BCMEVENT_NAME(WLC_E_AUTH),
	BCMEVENT_NAME(WLC_E_AUTH_IND),
	BCMEVENT_NAME(WLC_E_DEAUTH),
	BCMEVENT_NAME(WLC_E_DEAUTH_IND),
	BCMEVENT_NAME(WLC_E_ASSOC),
	BCMEVENT_NAME(WLC_E_ASSOC_IND),
	BCMEVENT_NAME(WLC_E_REASSOC),
	BCMEVENT_NAME(WLC_E_REASSOC_IND),
	BCMEVENT_NAME(WLC_E_DISASSOC),
	BCMEVENT_NAME(WLC_E_DISASSOC_IND),
	BCMEVENT_NAME(WLC_E_QUIET_START),
	BCMEVENT_NAME(WLC_E_QUIET_END),
	BCMEVENT_NAME(WLC_E_BEACON_RX),
	BCMEVENT_NAME(WLC_E_LINK),
	BCMEVENT_NAME(WLC_E_MIC_ERROR),
	BCMEVENT_NAME(WLC_E_NDIS_LINK),
	BCMEVENT_NAME(WLC_E_ROAM),
	BCMEVENT_NAME(WLC_E_TXFAIL),
	BCMEVENT_NAME(WLC_E_PMKID_CACHE),
	BCMEVENT_NAME(WLC_E_RETROGRADE_TSF),
	BCMEVENT_NAME(WLC_E_PRUNE),
	BCMEVENT_NAME(WLC_E_AUTOAUTH),
	BCMEVENT_NAME(WLC_E_EAPOL_MSG),
	BCMEVENT_NAME(WLC_E_SCAN_COMPLETE),
	BCMEVENT_NAME(WLC_E_ADDTS_IND),
	BCMEVENT_NAME(WLC_E_DELTS_IND),
	BCMEVENT_NAME(WLC_E_BCNSENT_IND),
	BCMEVENT_NAME(WLC_E_BCNRX_MSG),
	BCMEVENT_NAME(WLC_E_BCNLOST_MSG),
	BCMEVENT_NAME(WLC_E_ROAM_PREP),
	BCMEVENT_NAME(WLC_E_PFN_NET_FOUND),
	BCMEVENT_NAME(WLC_E_PFN_NET_LOST),
#if defined(IBSS_PEER_DISCOVERY_EVENT)
	BCMEVENT_NAME(WLC_E_IBSS_ASSOC),
#endif /* defined(IBSS_PEER_DISCOVERY_EVENT) */
	BCMEVENT_NAME(WLC_E_RADIO),
	BCMEVENT_NAME(WLC_E_PSM_WATCHDOG),
	BCMEVENT_NAME(WLC_E_PROBREQ_MSG),
	BCMEVENT_NAME(WLC_E_SCAN_CONFIRM_IND),
	BCMEVENT_NAME(WLC_E_PSK_SUP),
	BCMEVENT_NAME(WLC_E_COUNTRY_CODE_CHANGED),
	BCMEVENT_NAME(WLC_E_EXCEEDED_MEDIUM_TIME),
	BCMEVENT_NAME(WLC_E_ICV_ERROR),
	BCMEVENT_NAME(WLC_E_UNICAST_DECODE_ERROR),
	BCMEVENT_NAME(WLC_E_MULTICAST_DECODE_ERROR),
	BCMEVENT_NAME(WLC_E_TRACE),
	BCMEVENT_NAME(WLC_E_IF),
#ifdef WLP2P
	BCMEVENT_NAME(WLC_E_P2P_DISC_LISTEN_COMPLETE),
#endif
	BCMEVENT_NAME(WLC_E_RSSI),
	BCMEVENT_NAME(WLC_E_PFN_BEST_BATCHING),
	BCMEVENT_NAME(WLC_E_EXTLOG_MSG),
#ifdef WIFI_ACT_FRAME
	BCMEVENT_NAME(WLC_E_ACTION_FRAME),
	BCMEVENT_NAME(WLC_E_ACTION_FRAME_RX),
	BCMEVENT_NAME(WLC_E_ACTION_FRAME_COMPLETE),
#endif
#if defined(NDIS) && (NDISVER >= 0x0620)
	BCMEVENT_NAME(WLC_E_PRE_ASSOC_IND),
	BCMEVENT_NAME(WLC_E_PRE_REASSOC_IND),
	BCMEVENT_NAME(WLC_E_CHANNEL_ADOPTED),
	BCMEVENT_NAME(WLC_E_AP_STARTED),
	BCMEVENT_NAME(WLC_E_DFS_AP_STOP),
	BCMEVENT_NAME(WLC_E_DFS_AP_RESUME),
	BCMEVENT_NAME(WLC_E_ASSOC_IND_NDIS),
	BCMEVENT_NAME(WLC_E_REASSOC_IND_NDIS),
	BCMEVENT_NAME(WLC_E_ACTION_FRAME_RX_NDIS),
	BCMEVENT_NAME(WLC_E_AUTH_REQ),
	BCMEVENT_NAME(WLC_E_IBSS_COALESCE),
#endif /* NDIS && NDISVER >= 0x0620 */
	BCMEVENT_NAME(WLC_E_ESCAN_RESULT),
	BCMEVENT_NAME(WLC_E_ACTION_FRAME_OFF_CHAN_COMPLETE),
#ifdef WLP2P
	BCMEVENT_NAME(WLC_E_PROBRESP_MSG),
	BCMEVENT_NAME(WLC_E_P2P_PROBREQ_MSG),
#endif
#ifdef PROP_TXSTATUS
	BCMEVENT_NAME(WLC_E_FIFO_CREDIT_MAP),
#endif
	BCMEVENT_NAME(WLC_E_WAKE_EVENT),
	BCMEVENT_NAME(WLC_E_DCS_REQUEST),
	BCMEVENT_NAME(WLC_E_RM_COMPLETE),
#ifdef WLMEDIA_HTSF
	BCMEVENT_NAME(WLC_E_HTSFSYNC),
#endif
	BCMEVENT_NAME(WLC_E_OVERLAY_REQ),
	BCMEVENT_NAME(WLC_E_CSA_COMPLETE_IND),
	BCMEVENT_NAME(WLC_E_EXCESS_PM_WAKE_EVENT),
	BCMEVENT_NAME(WLC_E_PFN_SCAN_NONE),
	BCMEVENT_NAME(WLC_E_PFN_SCAN_ALLGONE),
#ifdef SOFTAP
	BCMEVENT_NAME(WLC_E_GTK_PLUMBED),
#endif
	BCMEVENT_NAME(WLC_E_ASSOC_REQ_IE),
	BCMEVENT_NAME(WLC_E_ASSOC_RESP_IE),
	BCMEVENT_NAME(WLC_E_BEACON_FRAME_RX),
#ifdef WLTDLS
	BCMEVENT_NAME(WLC_E_TDLS_PEER_EVENT),
#endif /* WLTDLS */
	BCMEVENT_NAME(WLC_E_NATIVE),
#ifdef WLPKTDLYSTAT
	BCMEVENT_NAME(WLC_E_PKTDELAY_IND),
#endif /* WLPKTDLYSTAT */
	BCMEVENT_NAME(WLC_E_SERVICE_FOUND),
	BCMEVENT_NAME(WLC_E_GAS_FRAGMENT_RX),
	BCMEVENT_NAME(WLC_E_GAS_COMPLETE),
	BCMEVENT_NAME(WLC_E_P2PO_ADD_DEVICE),
	BCMEVENT_NAME(WLC_E_P2PO_DEL_DEVICE),
#ifdef WLWNM
	BCMEVENT_NAME(WLC_E_WNM_STA_SLEEP),
#endif /* WLWNM */
#if defined(WL_PROXDETECT)
	BCMEVENT_NAME(WLC_E_PROXD),
#endif
#ifdef WLAWDL
	BCMEVENT_NAME(WLC_E_AWDL_AW_EXT_END),
	BCMEVENT_NAME(WLC_E_AWDL_AW_EXT_START),
	BCMEVENT_NAME(WLC_E_AWDL_AW_START),
	BCMEVENT_NAME(WLC_E_AWDL_RADIO_OFF),
	BCMEVENT_NAME(WLC_E_AWDL_PEER_STATE),
	BCMEVENT_NAME(WLC_E_AWDL_SYNC_STATE_CHANGED),
	BCMEVENT_NAME(WLC_E_AWDL_CHIP_RESET),
	BCMEVENT_NAME(WLC_E_AWDL_INTERLEAVED_SCAN_START),
	BCMEVENT_NAME(WLC_E_AWDL_INTERLEAVED_SCAN_STOP),
	BCMEVENT_NAME(WLC_E_AWDL_PEER_CACHE_CONTROL),
	BCMEVENT_NAME(WLC_E_CSA_START_IND),
	BCMEVENT_NAME(WLC_E_CSA_DONE_IND),
	BCMEVENT_NAME(WLC_E_CSA_FAILURE_IND),
#endif
	BCMEVENT_NAME(WLC_E_CCA_CHAN_QUAL),
	BCMEVENT_NAME(WLC_E_BSSID),
#ifdef PROP_TXSTATUS
	BCMEVENT_NAME(WLC_E_BCMC_CREDIT_SUPPORT),
#endif
	BCMEVENT_NAME(WLC_E_TXFAIL_THRESH),

	BCMEVENT_NAME(WLC_E_AIBSS_TXFAIL),
	BCMEVENT_NAME(WLC_E_CCA_CHAN_QUAL),
#ifdef WLBSSLOAD_REPORT
	BCMEVENT_NAME(WLC_E_BSS_LOAD),
#endif
#if 1
	BCMEVENT_NAME(WLC_E_AUTHORIZED),
	BCMEVENT_NAME(WLC_E_PROBREQ_MSG_RX),
#endif /* NETGEAR_PATCH */
};


const char *bcmevent_get_name(uint event_type)
{
	/* note:  first coded this as a static const but some
	 * ROMs already have something called event_name so
	 * changed it so we don't have a variable for the
	 * 'unknown string
	 */
	const char *event_name = NULL;

	uint idx;
	for (idx = 0; idx < (uint)ARRAYSIZE(bcmevent_names); idx++) {

		if (bcmevent_names[idx].event == event_type) {
			event_name = bcmevent_names[idx].name;
			break;
		}
	}

	/* if we find an event name in the array, return it.
	 * otherwise return unknown string.
	 */
	return ((event_name) ? event_name : "Unknown Event");
}
