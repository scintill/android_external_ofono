/*
 *
 *  oFono - Open Telephony stack for Linux
 *
 *  Copyright (C) 2008-2009  Intel Corporation. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
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

#ifndef __OFONO_DBUS_H
#define __OFONO_DBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dbus/dbus.h>

#define OFONO_SERVICE	"org.ofono"
#define OFONO_MANAGER_INTERFACE "org.ofono.Manager"
#define OFONO_MANAGER_PATH "/"
#define OFONO_MODEM_INTERFACE "org.ofono.Modem"
#define OFONO_CALL_BARRING_INTERFACE "org.ofono.CallBarring"

/* Essentially a{sv} */
#define OFONO_PROPERTIES_ARRAY_SIGNATURE DBUS_DICT_ENTRY_BEGIN_CHAR_AS_STRING \
					DBUS_TYPE_STRING_AS_STRING \
					DBUS_TYPE_VARIANT_AS_STRING \
					DBUS_DICT_ENTRY_END_CHAR_AS_STRING

DBusConnection *ofono_dbus_get_connection();

void ofono_dbus_dict_append(DBusMessageIter *dict, const char *key, int type,
				void *value);

void ofono_dbus_dict_append_array(DBusMessageIter *dict, const char *key,
					int type, void *val);

int ofono_dbus_signal_property_changed(DBusConnection *conn, const char *path,
					const char *interface, const char *name,
					int type, void *value);

int ofono_dbus_signal_array_property_changed(DBusConnection *conn,
						const char *path,
						const char *interface,
						const char *name, int type,
						void *value);

#ifdef __cplusplus
}
#endif

#endif /* __OFONO_DBUS_H */
