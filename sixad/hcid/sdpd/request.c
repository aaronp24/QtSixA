/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2001-2002  Nokia Corporation
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2002-2008  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2002-2003  Stephen Crane <steve.crane@rococosoft.com>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <malloc.h>
#include <limits.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>

#include "mod/sdp_lib.h"
//needs mod

#include <netinet/in.h>

#include "sdpd.h"
#include "logging.h"

#define MIN(x, y) ((x) < (y))? (x): (y)

/* Additional values for checking datatype (not in spec) */
#define SDP_TYPE_UUID	0xfe
#define SDP_TYPE_ANY	0xff

/*
 * Generic data element sequence extractor. Builds
 * a list whose elements are those found in the 
 * sequence. The data type of elements found in the
 * sequence is returned in the reference pDataType
 */
static int extract_des(uint8_t *buf, int len, sdp_list_t **svcReqSeq, uint8_t *pDataType, uint8_t expectedType)
{
	uint8_t seqType;
	int scanned, data_size = 0;
	short numberOfElements = 0;
	int seqlen = 0;
	sdp_list_t *pSeq = NULL;
	uint8_t dataType;
	int status = 0;
	const uint8_t *p;

	scanned = sdp_extract_seqtype(buf, &seqType, &data_size);

	debug("Seq type : %d", seqType);
	if (!scanned || (seqType != SDP_SEQ8 && seqType != SDP_SEQ16)) {
		error("Unknown seq type");
		return -1;
	}
	p = buf + scanned;

	debug("Data size : %d", data_size);

	for (;;) {
		char *pElem = NULL;
		int localSeqLength = 0;

		dataType = *(uint8_t *)p;
		debug("Data type: 0x%02x", dataType);

		if (expectedType == SDP_TYPE_UUID) {
			if (dataType != SDP_UUID16 && dataType != SDP_UUID32 && dataType != SDP_UUID128) {
				debug("->Unexpected Data type (expected UUID_ANY)");
				return -1;
			}
		} else if (expectedType != SDP_TYPE_ANY && dataType != expectedType) {
			debug("->Unexpected Data type (expected 0x%02x)", expectedType);
			return -1;
		}

		switch (dataType) {
		case SDP_UINT16:
			p += sizeof(uint8_t);
			seqlen += sizeof(uint8_t);
			pElem = malloc(sizeof(uint16_t));
			bt_put_unaligned(ntohs(bt_get_unaligned((uint16_t *)p)), (uint16_t *)pElem);
			p += sizeof(uint16_t);
			seqlen += sizeof(uint16_t);
			break;
		case SDP_UINT32:
			p += sizeof(uint8_t);
			seqlen += sizeof(uint8_t);
			pElem = malloc(sizeof(uint32_t));
			bt_put_unaligned(ntohl(bt_get_unaligned((uint32_t *)p)), (uint32_t *)pElem);
			p += sizeof(uint32_t);
			seqlen += sizeof(uint32_t);
			break;
		case SDP_UUID16:
		case SDP_UUID32:
		case SDP_UUID128:
			pElem = malloc(sizeof(uuid_t));
			status = sdp_uuid_extract(p, (uuid_t *)pElem, &localSeqLength);
			if (status == 0) {
				seqlen += localSeqLength;
				p += localSeqLength;
			}
			break;
		default:
			return -1;
		}
		if (status == 0) {
			pSeq = sdp_list_append(pSeq, pElem);
			numberOfElements++;
			debug("No of elements : %d", numberOfElements);

			if (seqlen == data_size)
				break;
			else if (seqlen > data_size || seqlen > len)
				return -1;
		} else
			free(pElem);
	}
	*svcReqSeq = pSeq;
	scanned += seqlen;
	*pDataType = dataType;
	return scanned;
}

static int sdp_set_cstate_pdu(sdp_buf_t *buf, sdp_cont_state_t *cstate)
{
	uint8_t *pdata = buf->data + buf->data_size;
	int length = 0;

	if (cstate) {
		debug("Non null sdp_cstate_t id : 0x%lx", cstate->timestamp);
		*(uint8_t *)pdata = sizeof(sdp_cont_state_t);
		pdata += sizeof(uint8_t);
		length += sizeof(uint8_t);
		memcpy(pdata, cstate, sizeof(sdp_cont_state_t));
		length += sizeof(sdp_cont_state_t);
	} else {
		// set "null" continuation state
		*(uint8_t *)pdata = 0;
		pdata += sizeof(uint8_t);
		length += sizeof(uint8_t);
	}
	buf->data_size += length;
	return length;
}

static sdp_cont_state_t *sdp_cstate_get(uint8_t *buffer)
{
	uint8_t *pdata = buffer;
	uint8_t cStateSize = *(uint8_t *)pdata;

	/*
	 * Check if continuation state exists, if yes attempt
	 * to get response remainder from cache, else send error
	 */
	debug("Continuation State size : %d", cStateSize);

	pdata += sizeof(uint8_t);
	if (cStateSize != 0) {
		sdp_cont_state_t *cstate = (sdp_cont_state_t *)pdata;
		debug("Cstate TS : 0x%lx", cstate->timestamp);
		debug("Bytes sent : %d", cstate->cStateValue.maxBytesSent);
		return cstate;
	}
	return NULL;
}

/*
 * The matching process is defined as "each and every UUID
 * specified in the "search pattern" must be present in the
 * "target pattern". Here "search pattern" is the set of UUIDs
 * specified by the service discovery client and "target pattern"
 * is the set of UUIDs present in a service record. 
 * 
 * Return 1 if each and every UUID in the search
 * pattern exists in the target pattern, 0 if the
 * match succeeds and -1 on error.
 */
static int sdp_match_uuid(sdp_list_t *search, sdp_list_t *pattern)
{
	/*
	 * The target is a sorted list, so we need not look
	 * at all elements to confirm existence of an element
	 * from the search pattern
	 */
	int patlen = sdp_list_len(pattern);

	if (patlen < sdp_list_len(search))
		return -1;
	for (; search; search = search->next) {
		uuid_t *uuid128;
		void *data = search->data;
		sdp_list_t *list;
		if (data == NULL)
			return -1;

		// create 128-bit form of the search UUID
		uuid128 = sdp_uuid_to_uuid128((uuid_t *)data);
		list = sdp_list_find(pattern, uuid128, sdp_uuid128_cmp);
		bt_free(uuid128);
		if (!list)
			return 0;
	}
	return 1;
}

/*
 * Service search request PDU. This method extracts the search pattern
 * (a sequence of UUIDs) and calls the matching function
 * to find matching services
 */
static int service_search_req(sdp_req_t *req, sdp_buf_t *buf)
{
	int status = 0, i, plen, mlen, mtu, scanned;
	sdp_list_t *pattern = NULL;
	uint16_t expected, actual;
	uint8_t dtd;
	sdp_cont_state_t *cstate = NULL;
	uint8_t *pCacheBuffer = NULL;
	int handleSize = 0;
	uint32_t cStateId = 0;
	short rsp_count = 0;
	short *pTotalRecordCount, *pCurrentRecordCount;
	uint8_t *pdata = req->buf + sizeof(sdp_pdu_hdr_t);

	scanned = extract_des(pdata, req->len - sizeof(sdp_pdu_hdr_t),
					&pattern, &dtd, SDP_TYPE_UUID);

	if (scanned == -1) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}
	pdata += scanned;

	plen = ntohs(((sdp_pdu_hdr_t *)(req->buf))->plen);
	mlen = scanned + sizeof(uint16_t) + 1;
	// ensure we don't read past buffer
	if (plen < mlen || plen != mlen + *(uint8_t *)(pdata+sizeof(uint16_t))) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}

	expected = ntohs(bt_get_unaligned((uint16_t *)pdata));
	
	debug("Expected count: %d", expected);
	debug("Bytes scanned : %d", scanned);

	pdata += sizeof(uint16_t);

	/*
	 * Check if continuation state exists, if yes attempt
	 * to get rsp remainder from cache, else send error
	 */
	cstate = sdp_cstate_get(pdata);

	mtu = req->mtu - sizeof(sdp_pdu_hdr_t) - sizeof(uint16_t) - sizeof(uint16_t) - SDP_CONT_STATE_SIZE;
	actual = MIN(expected, mtu >> 2);

	/* make space in the rsp buffer for total and current record counts */
	pdata = buf->data;

	/* total service record count = 0 */
	pTotalRecordCount = (short *)pdata;
	bt_put_unaligned(0, (uint16_t *)pdata);
	pdata += sizeof(uint16_t);
	buf->data_size += sizeof(uint16_t);

	/* current service record count = 0 */
	pCurrentRecordCount = (short *)pdata;
	bt_put_unaligned(0, (uint16_t *)pdata);
	pdata += sizeof(uint16_t);
	buf->data_size += sizeof(uint16_t);

	if (cstate == NULL) {
		/* for every record in the DB, do a pattern search */
		sdp_list_t *list = sdp_get_record_list();

		handleSize = 0;
		for (; list && rsp_count < expected; list = list->next) {
			sdp_record_t *rec = (sdp_record_t *) list->data;

			debug("Checking svcRec : 0x%x", rec->handle);
				
			if (sdp_match_uuid(pattern, rec->pattern) > 0 &&
					sdp_check_access(rec->handle, &req->device)) {
				rsp_count++;
				bt_put_unaligned(htonl(rec->handle), (uint32_t *)pdata);
				pdata += sizeof(uint32_t);
				handleSize += sizeof(uint32_t);
			}
		}
		
		debug("Match count: %d", rsp_count);

		buf->data_size += handleSize;
		bt_put_unaligned(htons(rsp_count), (uint16_t *)pTotalRecordCount);
		bt_put_unaligned(htons(rsp_count), (uint16_t *)pCurrentRecordCount);

		if (rsp_count > actual) {
			/* cache the rsp and generate a continuation state */
			cStateId = sdp_cstate_alloc_buf(buf);
			/*
			 * subtract handleSize since we now send only
			 * a subset of handles
			 */
			buf->data_size -= handleSize;
		} else {
			/* NULL continuation state */
			sdp_set_cstate_pdu(buf, NULL);
		}
	}

	/* under both the conditions below, the rsp buffer is not built yet */
	if (cstate || cStateId > 0) {
		short lastIndex = 0;

		if (cstate) {
			/*
			 * Get the previous sdp_cont_state_t and obtain
			 * the cached rsp
			 */
			sdp_buf_t *pCache = sdp_get_cached_rsp(cstate);
			if (pCache) {
				pCacheBuffer = pCache->data;
				/* get the rsp_count from the cached buffer */
				rsp_count = ntohs(bt_get_unaligned((uint16_t *)pCacheBuffer));

				/* get index of the last sdp_record_t sent */
				lastIndex = cstate->cStateValue.lastIndexSent;
			} else {
				status = SDP_INVALID_CSTATE;
				goto done;
			}
		} else {
			pCacheBuffer = buf->data;
			lastIndex = 0;
		}

		/*
		 * Set the local buffer pointer to after the
		 * current record count and increment the cached
		 * buffer pointer to beyond the counters
		 */
		pdata = (uint8_t *) pCurrentRecordCount + sizeof(uint16_t);

		/* increment beyond the totalCount and the currentCount */
		pCacheBuffer += 2 * sizeof(uint16_t);

		if (cstate) {
			handleSize = 0;
			for (i = lastIndex; (i - lastIndex) < actual && i < rsp_count; i++) {
				bt_put_unaligned(bt_get_unaligned((uint32_t *)(pCacheBuffer + i * sizeof(uint32_t))), (uint32_t *)pdata);
				pdata += sizeof(uint32_t);
				handleSize += sizeof(uint32_t);
			}
		} else {
			handleSize = actual << 2;
			i = actual;
		}

		buf->data_size += handleSize;
		bt_put_unaligned(htons(rsp_count), (uint16_t *)pTotalRecordCount);
		bt_put_unaligned(htons(i - lastIndex), (uint16_t *)pCurrentRecordCount);

		if (i == rsp_count) {
			/* set "null" continuationState */
			sdp_set_cstate_pdu(buf, NULL);
		} else {
			/*
			 * there's more: set lastIndexSent to
			 * the new value and move on
			 */
			sdp_cont_state_t newState;

			debug("Setting non-NULL sdp_cstate_t");

			if (cstate)
				memcpy((char *)&newState, cstate, sizeof(sdp_cont_state_t));
			else {
				memset((char *)&newState, 0, sizeof(sdp_cont_state_t));
				newState.timestamp = cStateId;
			}
			newState.cStateValue.lastIndexSent = i;
			sdp_set_cstate_pdu(buf, &newState);
		}
	}

done:	
	if (pattern)
		sdp_list_free(pattern, free);

	return status;
}

/*
 * Extract attribute identifiers from the request PDU.
 * Clients could request a subset of attributes (by id)
 * from a service record, instead of the whole set. The
 * requested identifiers are present in the PDU form of
 * the request
 */
static int extract_attrs(sdp_record_t *rec, sdp_list_t *seq, uint8_t dtd, sdp_buf_t *buf)
{
	if (!rec)
		return SDP_INVALID_RECORD_HANDLE;

	if (seq)
		debug("Entries in attr seq : %d", sdp_list_len(seq));
	else
		debug("NULL attribute descriptor");

	debug("AttrDataType : %d", dtd);

	if (seq == NULL) {
		debug("Attribute sequence is NULL");
		return 0;
	}
	if (dtd == SDP_UINT16)
		for (; seq; seq = seq->next) {
			uint16_t attr = bt_get_unaligned((uint16_t *)seq->data);
			sdp_data_t *a = (sdp_data_t *)sdp_data_get(rec, attr);
			if (a)
				sdp_append_to_pdu(buf, a);
		}
	else if (dtd == SDP_UINT32) {
		sdp_buf_t pdu;
		sdp_gen_record_pdu(rec, &pdu);
		for (; seq; seq = seq->next) {
			uint32_t range = bt_get_unaligned((uint32_t *)seq->data);
			uint16_t attr;
			uint16_t low = (0xffff0000 & range) >> 16;
			uint16_t high = 0x0000ffff & range;
			sdp_data_t *data;

			debug("attr range : 0x%x", range);
			debug("Low id : 0x%x", low);
			debug("High id : 0x%x", high);

			if (low == 0x0000 && high == 0xffff && pdu.data_size <= buf->buf_size) {
				/* copy it */
				memcpy(buf->data, pdu.data, pdu.data_size);
				buf->data_size = pdu.data_size;
				break;
			}
			/* (else) sub-range of attributes */
			for (attr = low; attr < high; attr++) {
				data = sdp_data_get(rec, attr);
				if (data)
					sdp_append_to_pdu(buf, data);
			}
			data = sdp_data_get(rec, high);
			if (data)
				sdp_append_to_pdu(buf, data);
		}
		free(pdu.data);
	} else {
		error("Unexpected data type : 0x%x", dtd);
		error("Expect uint16_t or uint32_t");
		return SDP_INVALID_SYNTAX;
	}
	return 0;
}

/*
 * A request for the attributes of a service record.
 * First check if the service record (specified by
 * service record handle) exists, then call the attribute
 * streaming function
 */
static int service_attr_req(sdp_req_t *req, sdp_buf_t *buf)
{
	sdp_cont_state_t *cstate = NULL;
	uint8_t *pResponse = NULL;
	short cstate_size = 0;
	sdp_list_t *seq = NULL;
	uint8_t dtd = 0;
	int scanned = 0;
	int max_rsp_size;
	int status = 0, plen, mlen;
	uint8_t *pdata = req->buf + sizeof(sdp_pdu_hdr_t);
	uint32_t handle = ntohl(bt_get_unaligned((uint32_t *)pdata));

	pdata += sizeof(uint32_t);
	max_rsp_size = ntohs(bt_get_unaligned((uint16_t *)pdata));
	pdata += sizeof(uint16_t);

	/* extract the attribute list */
	scanned = extract_des(pdata, req->len - sizeof(sdp_pdu_hdr_t),
						&seq, &dtd, SDP_TYPE_ANY);
	if (scanned == -1) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}
	pdata += scanned;

	plen = ntohs(((sdp_pdu_hdr_t *)(req->buf))->plen);
	mlen = scanned + sizeof(uint32_t) + sizeof(uint16_t) + 1;
	// ensure we don't read past buffer
	if (plen < mlen || plen != mlen + *(uint8_t *)pdata) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}

	/*
	 * if continuation state exists, attempt
	 * to get rsp remainder from cache, else send error
	 */
	cstate = sdp_cstate_get(pdata);

	debug("SvcRecHandle : 0x%x", handle);
	debug("max_rsp_size : %d", max_rsp_size);

	/* 
	 * Calculate Attribute size acording to MTU
	 * We can send only (MTU - sizeof(sdp_pdu_hdr_t) - sizeof(sdp_cont_state_t))
	 */
	max_rsp_size = MIN(max_rsp_size, req->mtu - sizeof(sdp_pdu_hdr_t) - 
			sizeof(uint32_t) - SDP_CONT_STATE_SIZE - sizeof(uint16_t));

	/* pull header for AttributeList byte count */
	buf->data += sizeof(uint16_t);
	buf->buf_size -= sizeof(uint16_t);

	if (cstate) {
		sdp_buf_t *pCache = sdp_get_cached_rsp(cstate);

		debug("Obtained cached rsp : %p", pCache);

		if (pCache) {
			short sent = MIN(max_rsp_size, pCache->data_size - cstate->cStateValue.maxBytesSent);
			pResponse = pCache->data;
			memcpy(buf->data, pResponse + cstate->cStateValue.maxBytesSent, sent);
			buf->data_size += sent;
			cstate->cStateValue.maxBytesSent += sent;

			debug("Response size : %d sending now : %d bytes sent so far : %d",
				pCache->data_size, sent, cstate->cStateValue.maxBytesSent);
			if (cstate->cStateValue.maxBytesSent == pCache->data_size)
				cstate_size = sdp_set_cstate_pdu(buf, NULL);
			else
				cstate_size = sdp_set_cstate_pdu(buf, cstate);
		} else {
			status = SDP_INVALID_CSTATE;
			error("NULL cache buffer and non-NULL continuation state");
		}
	} else {
		sdp_record_t *rec = sdp_record_find(handle);
		status = extract_attrs(rec, seq, dtd, buf);
		if (buf->data_size > max_rsp_size) {
			sdp_cont_state_t newState;

			memset((char *)&newState, 0, sizeof(sdp_cont_state_t));
			newState.timestamp = sdp_cstate_alloc_buf(buf);
			/*
			 * Reset the buffer size to the maximum expected and
			 * set the sdp_cont_state_t
			 */
			debug("Creating continuation state of size : %d", buf->data_size);
			buf->data_size = max_rsp_size;
			newState.cStateValue.maxBytesSent = max_rsp_size;
			cstate_size = sdp_set_cstate_pdu(buf, &newState);
		} else {
			if (buf->data_size == 0)
				sdp_append_to_buf(buf, 0, 0);
			cstate_size = sdp_set_cstate_pdu(buf, NULL);
		}
	}

	// push header
	buf->data -= sizeof(uint16_t);
	buf->buf_size += sizeof(uint16_t);

done:
	if (seq)
                sdp_list_free(seq, free);
	if (status)
		return status;

	/* set attribute list byte count */
	bt_put_unaligned(htons(buf->data_size - cstate_size), (uint16_t *)buf->data);
	buf->data_size += sizeof(uint16_t);
	return 0;
}

/*
 * combined service search and attribute extraction
 */
static int service_search_attr_req(sdp_req_t *req, sdp_buf_t *buf)
{
	int status = 0, plen, totscanned;
	uint8_t *pdata, *pResponse = NULL;
	int scanned, max, rsp_count = 0;
	sdp_list_t *pattern = NULL, *seq = NULL, *svcList;
	sdp_cont_state_t *cstate = NULL;
	short cstate_size = 0;
	uint8_t dtd = 0;
	sdp_buf_t tmpbuf;

	tmpbuf.data = NULL;
	pdata = req->buf + sizeof(sdp_pdu_hdr_t);
	scanned = extract_des(pdata, req->len - sizeof(sdp_pdu_hdr_t),
					&pattern, &dtd, SDP_TYPE_UUID);
	if (scanned == -1) {
        	status = SDP_INVALID_SYNTAX;
		goto done;
	}
	totscanned = scanned;

	debug("Bytes scanned: %d", scanned);

	pdata += scanned;
	max = ntohs(bt_get_unaligned((uint16_t *)pdata));
	pdata += sizeof(uint16_t);

	debug("Max Attr expected: %d", max);

	/* extract the attribute list */
	scanned = extract_des(pdata, req->len - sizeof(sdp_pdu_hdr_t),
						&seq, &dtd, SDP_TYPE_ANY);
	if (scanned == -1) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}
	pdata += scanned;
	totscanned += scanned + sizeof(uint16_t) + 1;

	plen = ntohs(((sdp_pdu_hdr_t *)(req->buf))->plen);
	if (plen < totscanned || plen != totscanned + *(uint8_t *)pdata) {
		status = SDP_INVALID_SYNTAX;
		goto done;
	}

	/*
	 * if continuation state exists attempt
	 * to get rsp remainder from cache, else send error
	 */
	cstate = sdp_cstate_get(pdata);	// continuation information

	svcList = sdp_get_record_list();

	tmpbuf.data = malloc(USHRT_MAX);
	tmpbuf.data_size = 0;
	tmpbuf.buf_size = USHRT_MAX;
	memset(tmpbuf.data, 0, USHRT_MAX);

	/* 
	 * Calculate Attribute size acording to MTU
	 * We can send only (MTU - sizeof(sdp_pdu_hdr_t) - sizeof(sdp_cont_state_t))
	 */
	max = MIN(max, req->mtu - sizeof(sdp_pdu_hdr_t) - SDP_CONT_STATE_SIZE - sizeof(uint16_t));

	/* pull header for AttributeList byte count */
	buf->data += sizeof(uint16_t);
	buf->buf_size -= sizeof(uint16_t);

	if (cstate == NULL) {
		/* no continuation state -> create new response */
		sdp_list_t *p;
		for (p = svcList; p; p = p->next) {
			sdp_record_t *rec = (sdp_record_t *) p->data;
			if (sdp_match_uuid(pattern, rec->pattern) > 0 &&
					sdp_check_access(rec->handle, &req->device)) {
				rsp_count++;
				status = extract_attrs(rec, seq, dtd, &tmpbuf);

				debug("Response count : %d", rsp_count);
				debug("Local PDU size : %d", tmpbuf.data_size);
				if (status) {
					debug("Extract attr from record returns err");
					break;
				}
				if (buf->data_size + tmpbuf.data_size < buf->buf_size) {
					// to be sure no relocations
					sdp_append_to_buf(buf, tmpbuf.data, tmpbuf.data_size);
					tmpbuf.data_size = 0;
					memset(tmpbuf.data, 0, USHRT_MAX);
				} else {
					error("Relocation needed");
					break;
				}
				debug("Net PDU size : %d", buf->data_size);
			}
		}
		if (buf->data_size > max) {
			sdp_cont_state_t newState;

			memset((char *)&newState, 0, sizeof(sdp_cont_state_t));
			newState.timestamp = sdp_cstate_alloc_buf(buf);
			/*
			 * Reset the buffer size to the maximum expected and
			 * set the sdp_cont_state_t
			 */
			buf->data_size = max;
			newState.cStateValue.maxBytesSent = max;
			cstate_size = sdp_set_cstate_pdu(buf, &newState);
		} else
			cstate_size = sdp_set_cstate_pdu(buf, NULL);
	} else {
		/* continuation State exists -> get from cache */
		sdp_buf_t *pCache = sdp_get_cached_rsp(cstate);
		if (pCache) {
			uint16_t sent = MIN(max, pCache->data_size - cstate->cStateValue.maxBytesSent);
			pResponse = pCache->data;
			memcpy(buf->data, pResponse + cstate->cStateValue.maxBytesSent, sent);
			buf->data_size += sent;
			cstate->cStateValue.maxBytesSent += sent;
			if (cstate->cStateValue.maxBytesSent == pCache->data_size)
				cstate_size = sdp_set_cstate_pdu(buf, NULL);
			else
				cstate_size = sdp_set_cstate_pdu(buf, cstate);
		} else {
			status = SDP_INVALID_CSTATE;
			debug("Non-null continuation state, but null cache buffer");
		}
	}

	if (!rsp_count && !cstate) {
		// found nothing
		buf->data_size = 0;
		sdp_append_to_buf(buf, tmpbuf.data, tmpbuf.data_size);
		sdp_set_cstate_pdu(buf, NULL);
	}

	// push header
	buf->data -= sizeof(uint16_t);
	buf->buf_size += sizeof(uint16_t);

	if (!status) {
		/* set attribute list byte count */
		bt_put_unaligned(htons(buf->data_size - cstate_size), (uint16_t *)buf->data);
		buf->data_size += sizeof(uint16_t);
	}

done:
	if (tmpbuf.data)
		free(tmpbuf.data);
	if (pattern)
		sdp_list_free(pattern, free);
	if (seq)
                sdp_list_free(seq, free);
	return status;
}

/*
 * Top level request processor. Calls the appropriate processing
 * function based on request type. Handles service registration
 * client requests also.
 */
static void process_request(sdp_req_t *req)
{
	sdp_pdu_hdr_t *reqhdr = (sdp_pdu_hdr_t *)req->buf;
	sdp_pdu_hdr_t *rsphdr;
	sdp_buf_t rsp;
	uint8_t *buf = malloc(USHRT_MAX);
	int sent = 0;
	int status = SDP_INVALID_SYNTAX;

	memset(buf, 0, USHRT_MAX);
	rsp.data = buf + sizeof(sdp_pdu_hdr_t);
	rsp.data_size = 0;
	rsp.buf_size = USHRT_MAX - sizeof(sdp_pdu_hdr_t);
	rsphdr = (sdp_pdu_hdr_t *)buf;

	if (ntohs(reqhdr->plen) != req->len - sizeof(sdp_pdu_hdr_t)) {
		status = SDP_INVALID_PDU_SIZE;
		goto send_rsp;
	}
	switch (reqhdr->pdu_id) {
	case SDP_SVC_SEARCH_REQ:
		debug("Got a svc srch req");
		status = service_search_req(req, &rsp);
		rsphdr->pdu_id = SDP_SVC_SEARCH_RSP;
		break;
	case SDP_SVC_ATTR_REQ:
		debug("Got a svc attr req");
		status = service_attr_req(req, &rsp);
		rsphdr->pdu_id = SDP_SVC_ATTR_RSP;
		break;
	case SDP_SVC_SEARCH_ATTR_REQ:
		debug("Got a svc srch attr req");
		status = service_search_attr_req(req, &rsp);
		rsphdr->pdu_id = SDP_SVC_SEARCH_ATTR_RSP;
		break;
	/* Following requests are allowed only for local connections */
	case SDP_SVC_REGISTER_REQ:
		debug("Service register request");
		if (req->local) {
			status = service_register_req(req, &rsp);
			rsphdr->pdu_id = SDP_SVC_REGISTER_RSP;
		}
		break;
	case SDP_SVC_UPDATE_REQ:
		debug("Service update request");
		if (req->local) {
			status = service_update_req(req, &rsp);
			rsphdr->pdu_id = SDP_SVC_UPDATE_RSP;
		}
		break;
	case SDP_SVC_REMOVE_REQ:
		debug("Service removal request");
		if (req->local) {
			status = service_remove_req(req, &rsp);
			rsphdr->pdu_id = SDP_SVC_REMOVE_RSP;
		}
		break;
	default:
		error("Unknown PDU ID : 0x%x received", reqhdr->pdu_id);
		status = SDP_INVALID_SYNTAX;
		break;
	}

send_rsp:
	if (status) {
		rsphdr->pdu_id = SDP_ERROR_RSP;
		bt_put_unaligned(htons(status), (uint16_t *)rsp.data);
		rsp.data_size = sizeof(uint16_t);
	}
	
	debug("Sending rsp. status %d", status);

	rsphdr->tid  = reqhdr->tid;
	rsphdr->plen = htons(rsp.data_size);

	/* point back to the real buffer start and set the real rsp length */
	rsp.data_size += sizeof(sdp_pdu_hdr_t);
	rsp.data = buf;

	/* stream the rsp PDU */
	sent = send(req->sock, rsp.data, rsp.data_size, 0);

	debug("Bytes Sent : %d", sent);

	free(rsp.data);
	free(req->buf);
}

void handle_request(int sk, uint8_t *data, int len)
{
	struct sockaddr_l2 sa;
	socklen_t size;
	sdp_req_t req;

	size = sizeof(sa);
	if (getpeername(sk, (struct sockaddr *) &sa, &size) < 0)
		return;

	if (sa.l2_family == AF_BLUETOOTH) { 
		struct l2cap_options lo;
		memset(&lo, 0, sizeof(lo));
		size = sizeof(lo);
		getsockopt(sk, SOL_L2CAP, L2CAP_OPTIONS, &lo, &size);
		bacpy(&req.bdaddr, &sa.l2_bdaddr);
		req.mtu = lo.omtu;
		req.local = 0;
		memset(&sa, 0, sizeof(sa));
		size = sizeof(sa);
		getsockname(sk, (struct sockaddr *) &sa, &size);
		bacpy(&req.device, &sa.l2_bdaddr);
	} else {
		bacpy(&req.device, BDADDR_ANY);
		bacpy(&req.bdaddr, BDADDR_LOCAL);
		req.mtu = 2048;
		req.local = 1;
	}

	req.sock = sk;
	req.buf  = data;
	req.len  = len;

	process_request(&req);
}
