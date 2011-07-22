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
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <sys/un.h>
#include <netinet/in.h>

#include <glib.h>

#include "logging.h"
#include "sdpd.h"

static GIOChannel *l2cap_io = NULL, *unix_io = NULL;

static int l2cap_sock, unix_sock;

/*
 * SDP server initialization on startup includes creating the
 * l2cap and unix sockets over which discovery and registration clients
 * access us respectively
 */
static int init_server(uint16_t mtu, int master, int compat)
{
	struct l2cap_options opts;
	struct sockaddr_l2 l2addr;
	struct sockaddr_un unaddr;
	socklen_t optlen;

	/* Register the public browse group root */
	register_public_browse_group();

	/* Register the SDP server's service record */
	register_server_service();

	/* Create L2CAP socket */
	l2cap_sock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (l2cap_sock < 0) {
		error("opening L2CAP socket: %s", strerror(errno));
		return -1;
	}

	memset(&l2addr, 0, sizeof(l2addr));
	l2addr.l2_family = AF_BLUETOOTH;
	bacpy(&l2addr.l2_bdaddr, BDADDR_ANY);
	l2addr.l2_psm = htobs(SDP_PSM);

	if (bind(l2cap_sock, (struct sockaddr *) &l2addr, sizeof(l2addr)) < 0) {
		error("binding L2CAP socket: %s", strerror(errno));
		return -1;
	}

	if (master) {
		int opt = L2CAP_LM_MASTER;
		if (setsockopt(l2cap_sock, SOL_L2CAP, L2CAP_LM, &opt, sizeof(opt)) < 0) {
			error("setsockopt: %s", strerror(errno));
			return -1;
		}
	}

	if (mtu > 0) {
		memset(&opts, 0, sizeof(opts));
		optlen = sizeof(opts);

		if (getsockopt(l2cap_sock, SOL_L2CAP, L2CAP_OPTIONS, &opts, &optlen) < 0) {
			error("getsockopt: %s", strerror(errno));
			return -1;
		}

		opts.omtu = mtu;
		opts.imtu = mtu;

		if (setsockopt(l2cap_sock, SOL_L2CAP, L2CAP_OPTIONS, &opts, sizeof(opts)) < 0) {
			error("setsockopt: %s", strerror(errno));
			return -1;
		}
	}

	listen(l2cap_sock, 5);

	if (!compat) {
		unix_sock = -1;
		return 0;
	}

	/* Create local Unix socket */
	unix_sock = socket(PF_UNIX, SOCK_STREAM, 0);
	if (unix_sock < 0) {
		error("opening UNIX socket: %s", strerror(errno));
		return -1;
	}

	memset(&unaddr, 0, sizeof(unaddr));
	unaddr.sun_family = AF_UNIX;
	strcpy(unaddr.sun_path, SDP_UNIX_PATH);

	unlink(unaddr.sun_path);

	if (bind(unix_sock, (struct sockaddr *) &unaddr, sizeof(unaddr)) < 0) {
		error("binding UNIX socket: %s", strerror(errno));
		return -1;
	}

	listen(unix_sock, 5);

	chmod(SDP_UNIX_PATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

	return 0;
}

static gboolean io_session_event(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	sdp_pdu_hdr_t hdr;
	uint8_t *buf;
	int sk, len, size;

	if (cond & (G_IO_HUP | G_IO_ERR | G_IO_NVAL))
		return FALSE;

	sk = g_io_channel_unix_get_fd(chan);

	len = recv(sk, &hdr, sizeof(sdp_pdu_hdr_t), MSG_PEEK);
	if (len <= 0) {
		sdp_svcdb_collect_all(sk);
		return FALSE;
	}

	size = sizeof(sdp_pdu_hdr_t) + ntohs(hdr.plen);
	buf = malloc(size);
	if (!buf)
		return TRUE;

	len = recv(sk, buf, size, 0);
	if (len <= 0) {
		sdp_svcdb_collect_all(sk);
		return FALSE;
	}
		
	handle_request(sk, buf, len);

	return TRUE;
}

static gboolean io_accept_event(GIOChannel *chan, GIOCondition cond, gpointer data)
{
	GIOChannel *io;
	int nsk;

	if (cond & (G_IO_HUP | G_IO_ERR | G_IO_NVAL)) {
		g_io_channel_unref(chan);
		return FALSE;
	}

	if (data == &l2cap_sock) {
		struct sockaddr_l2 addr;
		socklen_t len = sizeof(addr);

		nsk = accept(l2cap_sock, (struct sockaddr *) &addr, &len);
	} else if (data == &unix_sock) {
		struct sockaddr_un addr;
		socklen_t len = sizeof(addr);

		nsk = accept(unix_sock, (struct sockaddr *) &addr, &len);
	} else
		return FALSE;

	if (nsk < 0) {
		error("Can't accept connection: %s", strerror(errno));
		return TRUE;
	}

	io = g_io_channel_unix_new(nsk);
	g_io_channel_set_close_on_unref(io, TRUE);

	g_io_add_watch(io, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
					io_session_event, data);

	g_io_channel_unref(io);

	return TRUE;
}

int start_sdp_server(uint16_t mtu, const char *did, uint32_t flags)
{
	int compat = flags & SDP_SERVER_COMPAT;
	int master = flags & SDP_SERVER_MASTER;

	info("Starting SDP server");

	if (init_server(mtu, master, compat) < 0) {
		error("Server initialization failed");
		return -1;
	}

	if (did && strlen(did) > 0) {
		const char *ptr = did;
		uint16_t vid = 0x0000, pid = 0x0000, ver = 0x0000;

		vid = (uint16_t) strtol(ptr, NULL, 16);
		ptr = strchr(ptr, ':');
		if (ptr) {
			pid = (uint16_t) strtol(ptr + 1, NULL, 16);
			ptr = strchr(ptr + 1, ':');
			if (ptr)
				ver = (uint16_t) strtol(ptr + 1, NULL, 16);
			register_device_id(vid, pid, ver);
		}
	}

	l2cap_io = g_io_channel_unix_new(l2cap_sock);
	g_io_channel_set_close_on_unref(l2cap_io, TRUE);

	g_io_add_watch(l2cap_io, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
					io_accept_event, &l2cap_sock);

	if (compat && unix_sock > fileno(stderr)) {
		unix_io = g_io_channel_unix_new(unix_sock);
		g_io_channel_set_close_on_unref(unix_io, TRUE);

		g_io_add_watch(unix_io, G_IO_IN | G_IO_ERR | G_IO_HUP | G_IO_NVAL,
					io_accept_event, &unix_sock);
	}

	return 0;
}

void stop_sdp_server(void)
{
	info("Stopping SDP server");

	sdp_svcdb_reset();

	if (unix_io)
		g_io_channel_unref(unix_io);

	if (l2cap_io)
		g_io_channel_unref(l2cap_io);
}
