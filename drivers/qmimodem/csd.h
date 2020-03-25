/*
 *
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2020 Joey Hewitt <joey@joeyhewitt.com>
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

typedef uint32_t qmi_csd_device_id;
#define QMI_CSD_DEVICE_ID_DBUS_TYPE DBUS_TYPE_UINT32
typedef uint32_t qmi_csd_handle;

struct qmi_csd_enable_devices {
	uint8_t _2; // number of devices?
	struct {
		qmi_csd_device_id device_id;
		uint32_t sample_rate;
		uint32_t _16; // bits per sample?
	} rx, tx;
} __attribute__((__packed__));

struct qmi_csd_disable_devices {
	uint8_t _2; // number of devices?
	qmi_csd_device_id rx_device_id, tx_device_id;
} __attribute__((__packed__));

struct qmi_csd_open_context_full {
	char _x13_default_modem_voice[20];
	uint32_t _2;
	uint32_t _x10037;
} __attribute__((__packed__));

struct qmi_csd_set_device_config {
	uint32_t _0;
	qmi_csd_device_id tx_device_id, rx_device_id;
	uint32_t tx_sample_rate, rx_sample_rate;
} __attribute__((__packed__));

struct qmi_csd_attach_or_detach_context {
	uint32_t _0;
	qmi_csd_handle context_handle;
} __attribute__((__packed__));

struct qmi_csd_set_rx_volume {
	uint32_t _0;
	uint32_t volume_index;
} __attribute__((__packed__));

struct qmi_csd_set_stream_mute {
	uint32_t _0;
	uint32_t stream_num;
	uint32_t _0_2; // TODO this might be mute status
} __attribute__((__packed__));

void qmi_csd_register(struct qmi_device *device, const char *modem_path);

#define QMI_CSD_INIT                  0x21
#define QMI_CSD_OPEN_STREAM_PASSIVE   0x23
#define QMI_CSD_OPEN_CONTEXT_FULL     0x25
#define QMI_CSD_OPEN_MANAGER_PASSIVE  0x26
#define QMI_CSD_OPEN_DEVICE_CONTROL   0x27
#define QMI_CSD_CLOSE_CONTEXT_FULL    0x28
#define QMI_CSD_ENABLE_DEVICES        0x29
#define QMI_CSD_DISABLE_DEVICES       0x2a
#define QMI_CSD_SET_DEVICE_CONFIG     0x33
#define QMI_CSD_ENABLE_CONTEXT        0x34
#define QMI_CSD_DISABLE_CONTEXT       0x35
#define QMI_CSD_SET_RX_VOLUME         0x36
#define QMI_CSD_SET_STREAM_MUTE       0x3e
#define QMI_CSD_ATTACH_CONTEXT        0x55
#define QMI_CSD_DETACH_CONTEXT        0x56
#define QMI_CSD_START_MANAGER         0x57
#define QMI_CSD_STOP_MANAGER          0x58
