/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2006-2007  Nokia Corporation
 *  Copyright (C) 2004-2008  Marcel Holtmann <marcel@holtmann.org>
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

typedef enum {
	SDP_FORMAT_XML,
	SDP_FORMAT_BINARY
} sdp_format_t;

DBusHandlerResult get_remote_svc_handles(DBusConnection *conn,
						DBusMessage *msg, void *data);

DBusHandlerResult get_remote_svc_identifiers(DBusConnection *conn,
						DBusMessage *msg, void *data);

DBusHandlerResult get_remote_svc_rec(DBusConnection *conn, DBusMessage *msg,
					void *data, sdp_format_t format);

DBusHandlerResult finish_remote_svc_transact(DBusConnection *conn,
						DBusMessage *msg, void *data);

uint16_t sdp_str2svclass(const char *str);
