/*
 *
 *  oFono - Open Source Telephony
 *
 *  Copyright (C) 2011-2012  Intel Corporation. All rights reserved.
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

#define QMI_WMS_RESET			0	/* Reset WMS service */
#define QMI_WMS_EVENT			1	/* New message indication */
#define QMI_WMS_SET_EVENT		1	/* Set new message conditions */

#define QMI_WMS_RAW_SEND		32	/* Send a raw message */
#define QMI_WMS_RAW_READ		34	/* Read a raw message */
#define QMI_WMS_DELETE_MESSAGE		36	/* Delete a message */

#define QMI_WMS_GET_MSG_LIST		49	/* Get list of messages from the device */
#define QMI_WMS_SET_ROUTES		50	/* Set routes for message memory storage */
#define QMI_WMS_GET_ROUTES		51	/* Get routes for message memory storage */
#define QMI_WMS_GET_SMSC_ADDR		52	/* Get SMSC address */
#define QMI_WMS_SET_SMSC_ADDR		53	/* Set SMSC address */
#define QMI_WMS_GET_MSG_LIST_MAX	54	/* Get maximum size of SMS storage */

#define QMI_WMS_GET_DOMAIN_PREF		64	/* Get domain preference */
#define QMI_WMS_SET_DOMAIN_PREF		65	/* Set domain preference */


/* New message indication */
#define QMI_WMS_RESULT_NEW_MSG_NOTIFY		0x10
struct qmi_wms_result_new_msg_notify {
	uint8_t storage_type;
	uint32_t storage_index;
} __attribute__((__packed__));

/* Set new message conditions */
#define QMI_WMS_PARAM_NEW_MSG_REPORT		0x10	/* bool */

/* Send a raw message */
#define QMI_WMS_PARAM_MESSAGE			0x01
struct qmi_wms_param_message {
	uint8_t msg_format;
	uint16_t msg_length;
	uint8_t msg_data[0];
} __attribute__((__packed__));
#define QMI_WMS_RESULT_MESSAGE_ID		0x01	/* uint16 */

#define QMI_WMS_STORAGE_TYPE_UIM		0
#define QMI_WMS_STORAGE_TYPE_NV			1
#define QMI_WMS_STORAGE_TYPE_UNKNOWN		2
#define QMI_WMS_STORAGE_TYPE_NONE		255

#define QMI_WMS_MESSAGE_MODE_GSMWCDMA		1

#define QMI_WMS_RESULT_MESSAGE_LIST		0x01
struct qmi_wms_message_list {
	uint32_t count;
	struct {
		uint32_t memory_index;
		uint8_t message_tag;
	} __attribute__((__packed__)) message[0];
} __attribute__((__packed__));

enum qmi_wms_message_tag {
	QMI_WMS_MESSAGE_TAG_TYPE_MT_READ     = 0x00,
	QMI_WMS_MESSAGE_TAG_TYPE_MT_NOT_READ = 0x01,
	QMI_WMS_MESSAGE_TAG_TYPE_MO_SENT     = 0x02,
	QMI_WMS_MESSAGE_TAG_TYPE_MO_NOT_SENT = 0x03
};

/* read a message */
enum qmi_wms_param_read_message_req {
	QMI_WMS_PARAM_RAW_READ_MESSAGE_ID	= 0x01,
	QMI_WMS_PARAM_RAW_READ_MESSAGE_MODE	= 0x10,
};
enum qmi_wms_param_read_message_res {
	QMI_WMS_PARAM_RAW_READ_MESSAGE	= 0x01,
};
struct qmi_wms_raw_message {
	uint8_t message_tag;
	uint8_t message_format;
	uint16_t count;
	uint8_t data[0];
} __attribute__((__packed__));

/* delete a message */
enum qmi_wms_params_delete_req {
	QMI_WMS_PARAM_DELETE_STORAGE		= 0x01,
	QMI_WMS_PARAM_DELETE_MESSAGE_INDEX	= 0x10,
	QMI_WMS_PARAM_DELETE_MESSAGE_MODE	= 0x12,
};

/* Get list of messages from the device response */
enum qmi_wms_params_list_message_req {
	QMI_WMS_PARAM_LIST_MESSAGE_STORAGE_TYPE	= 0x01,
	QMI_WMS_PARAM_LIST_MESSAGE_TAG		= 0x11,
	QMI_WMS_PARAM_LIST_MESSAGE_MODE		= 0x12,
};

enum qmi_wms_params_list_message_res {
	QMI_WMS_PARAM_LIST_MESSAGE_LIST		= 0x01,
};

/* Get routes for message memory storage */
#define QMI_WMS_RESULT_ROUTE_LIST		0x01
#define QMI_WMS_PARAM_ROUTE_LIST		0x01
struct qmi_wms_route_list {
	uint16_t count;
	struct {
		uint8_t msg_type;
		uint8_t msg_class;
		uint8_t storage_type;
		uint8_t action;
	} __attribute__((__packed__)) route[0];
} __attribute__((__packed__));
#define QMI_WMS_RESULT_STATUS_REPORT		0x10	/* bool */
#define QMI_WMS_PARAM_STATUS_REPORT		0x10	/* bool */
#define QMI_WMS_RESULT_MESSAGE			0x11
struct qmi_wms_result_message {
	uint8_t ack_required;				/* bool */
	uint32_t transaction_id;
	uint8_t msg_format;
	uint16_t msg_length;
	uint8_t msg_data[0];
} __attribute__((__packed__));

#define QMI_WMS_MSG_TYPE_P2P			0x00
#define QMI_WMS_MSG_TYPE_BROADCAST		0x01

#define QMI_WMS_MSG_CLASS_0			0x00
#define QMI_WMS_MSG_CLASS_1			0x01
#define QMI_WMS_MSG_CLASS_2			0x02
#define QMI_WMS_MSG_CLASS_3			0x03
#define QMI_WMS_MSG_CLASS_NONE			0x04
#define QMI_WMS_MSG_CLASS_CDMA			0x05

#define QMI_WMS_ACTION_DISCARD			0x00
#define QMI_WMS_ACTION_STORE_AND_NOTIFY		0x01
#define QMI_WMS_ACTION_TRANSFER_ONLY		0x02
#define QMI_WMS_ACTION_TRANSFER_AND_ACK		0x03
#define QMI_WMS_ACTION_UNKNOWN			0xff

/* Get SMSC address */
#define QMI_WMS_RESULT_SMSC_ADDR		0x01
struct qmi_wms_result_smsc_addr {
	char type[3];
	uint8_t addr_len;
	char addr[0];
} __attribute__((__packed__));

/* Set SMSC address */
#define QMI_WMS_PARAM_SMSC_ADDR			0x01	/* string */
#define QMI_WMS_PARAM_SMSC_ADDR_TYPE		0x10	/* string */

/* Get domain preference */
#define QMI_WMS_RESULT_DOMAIN			0x01	/* uint8 */
#define QMI_WMS_PARAM_DOMAIN			0x01	/* uint8 */

#define QMI_WMS_DOMAIN_CS_PREFERRED		0x00
#define QMI_WMS_DOMAIN_PS_PREFERRED		0x01
#define QMI_WMS_DOMAIN_CS_ONLY			0x02
#define QMI_WMS_DOMAIN_PS_ONLY			0x03
