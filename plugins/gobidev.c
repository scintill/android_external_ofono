/*
 *
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2008-2011  Intel Corporation. All rights reserved.
 *  Copyright (C) 2014 Canonical Ltd.
 *  Copyright (C) 2017 Joey Hewitt <joey@joeyhewitt.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#include <glib.h>
#include <string.h>
#include <stdio.h>

#define OFONO_API_SUBJECT_TO_CHANGE
#include <ofono/plugin.h>
#include <ofono/modem.h>
#include <ofono/log.h>
#include <drivers/qmimodem/qmimodem.h>

static GSList *modem_list;

static int create_gobimodem(const char *devpath, const char *iface_name, int set_rawip)
{
	struct ofono_modem *modem;
	int retval;

	modem = ofono_modem_create(NULL, "gobi");
	if (modem == NULL) {
		DBG("ofono_modem_create failed");
		return -ENODEV;
	}

	modem_list = g_slist_prepend(modem_list, modem);

	ofono_modem_set_string(modem, "Device", devpath);
	ofono_modem_set_string(modem, "NetworkInterface", iface_name);
	ofono_modem_set_boolean(modem, MODEM_PROP_SET_RAWIP, set_rawip);

	/* This causes driver->probe() to be called... */
	retval = ofono_modem_register(modem);
	if (retval != 0) {
		ofono_error("%s: ofono_modem_register returned: %d",
				__func__, retval);
		return retval;
	}

	// TODO is this desirable for Android gobi?
	/*
	 * kickstart the modem:
	 * causes core modem code to call
	 * - set_powered(TRUE) - which in turn
	 *   calls driver->enable()
	 *
	 * - driver->pre_sim()
	 *
	 * Could also be done via:
	 *
	 * - a DBus call to SetProperties w/"Powered=TRUE" *1
	 * - sim_state_watch ( handles SIM removal? LOCKED states? **2
	 * - ofono_modem_set_powered()
	 */
	ofono_modem_reset(modem);

	return 0;
}

static int detect_init(void)
{
	const char *gobi_path;
	const char *iface_name;
	int set_rawip = 0;

	gobi_path = getenv("OFONO_GOBI_DEVICE");
	if (gobi_path == NULL)
		return 0;

	iface_name = getenv("OFONO_GOBI_IFACE");
	if (iface_name == NULL)
		return 0;

	set_rawip = (getenv("OFONO_GOBI_SET_RAWIP") != NULL);

	ofono_info("GobiDev initializing modem path %s",
			gobi_path);

	create_gobimodem(gobi_path, iface_name, set_rawip);

	return 0;
}

static void detect_exit(void)
{
	GSList *list;

	for (list = modem_list; list; list = list->next) {
		struct ofono_modem *modem = list->data;

		ofono_modem_remove(modem);
	}

	g_slist_free(modem_list);
	modem_list = NULL;
}

OFONO_PLUGIN_DEFINE(gobidev, "udev-less gobi initialization", VERSION,
		OFONO_PLUGIN_PRIORITY_DEFAULT, detect_init, detect_exit)
