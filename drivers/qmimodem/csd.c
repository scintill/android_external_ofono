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
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// Plugin to manipulate the Qualcomm CSD service (Core Sound Driver)

#include <gdbus.h>
#include <inttypes.h>
#include "ofono.h"
#include "qmi.h"
#include "util.h"
#include "csd.h"

#define CSD_INTERFACE_NAME "org.ofono.QualcommCoreSoundDriver_Beta1"

// TODO parameterize these
#define SAMPLE_RATE 16000
static const uint32_t rx_volume_index = 4;

// assume 0 is an invalid handle, but use this macro to flag those assumptions
#define HANDLE_UNSET 0

// Global service state tracking
struct csd_data {
	struct qmi_service *svc;
	struct DBusMessage *pending;
	qmi_csd_handle device_control_handle, stream_handle, manager_handle, context_handle;
	enum {
		csd_status_uninited = 0,
		csd_status_inited,
		csd_status_started,
	} status;
	qmi_csd_device_id tx_device_id, rx_device_id;
};

// Each operation has a statemachine func, which processes QMI responses and sends further QMI commands, or a dbus response.
struct csd_cb_data;
typedef int (*csd_statemachine_func)(struct csd_data *, struct csd_cb_data *, struct qmi_result *, struct qmi_param *);

// Callback data for QMI callbacks
struct csd_cb_data {
	const char *operation;
	csd_statemachine_func statemachine_func;
	unsigned int state;
	struct csd_data *csd;
};

// Helper to dispatch a dbus reply
static void dbus_reply(struct csd_cb_data *cb_data, struct qmi_param *param, DBusMessage* (*replyfunc)(DBusMessage*)) {
	struct csd_data *csd = cb_data->csd;
	qmi_param_free(param); // a dbus reply is sent when either nothing was sent in this state, or it failed to send and was thus not freed by qmi_service_send()
	DBusMessage *reply = replyfunc(csd->pending);
    __ofono_dbus_pending_reply(&csd->pending, reply);
    g_free(cb_data);
}

static void statemachine(struct qmi_result *result, void *_cb_data);

// Init operation
static int statemachine_for_init(struct csd_data *csd, struct csd_cb_data *cb_data, struct qmi_result *result, struct qmi_param *param) {
	switch (cb_data->state) {
		case 1: {
			if (!qmi_service_send(csd->svc, QMI_CSD_INIT, NULL, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 2: {
			if (!qmi_service_send(csd->svc, QMI_CSD_OPEN_DEVICE_CONTROL, NULL, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 3: {
			if (!qmi_result_get_uint32(result, 0x11, &csd->device_control_handle)) {
				DBG("no device control handle returned");
				return -1;
			}
			DBG("device_control_handle = 0x%"PRIx32, csd->device_control_handle);
			qmi_param_append(param, 1, 19, "default modem voice");
			if (!qmi_service_send(csd->svc, QMI_CSD_OPEN_STREAM_PASSIVE, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 4: {
			if (!qmi_result_get_uint32(result, 0x11, &csd->stream_handle)) {
				DBG("no stream handle returned");
				return -1;
			}
			DBG("stream_handle = 0x%"PRIx32, csd->stream_handle);
			qmi_param_append(param, 1, 20, "\x13" "default modem voice");
			if (!qmi_service_send(csd->svc, QMI_CSD_OPEN_MANAGER_PASSIVE, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 5: {
			if (!qmi_result_get_uint32(result, 0x11, &csd->manager_handle)) {
				DBG("no manager handle returned");
				return -1;
			}
			DBG("manager_handle = 0x%"PRIx32, csd->manager_handle);
			csd->status = csd_status_inited;
			dbus_reply(cb_data, param, dbus_message_new_method_return);
			return 0;
		}
		default:
			return -1;
	}
}

// Start operation
static int statemachine_for_start(struct csd_data *csd, struct csd_cb_data *cb_data, struct qmi_result *result, struct qmi_param *param) {
	switch (cb_data->state) {
		case 1: {
			const struct qmi_csd_enable_devices devices = {
					._2 = 2,
					.rx = { .device_id = csd->rx_device_id, .sample_rate = SAMPLE_RATE, ._16 = 16 },
					.tx = { .device_id = csd->tx_device_id, .sample_rate = SAMPLE_RATE, ._16 = 16 },
			};
			qmi_param_append_uint32(param, 1, csd->device_control_handle);
			qmi_param_append(param, 2, sizeof(devices), &devices);
			if (!qmi_service_send(csd->svc, QMI_CSD_ENABLE_DEVICES, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 2: {
			const struct qmi_csd_open_context_full open_context = {
					._x13_default_modem_voice = "\x13" "default modem voice",
					._2 = 2, ._x10037 = 0x10037,
			};
			qmi_param_append(param, 1, sizeof(open_context), &open_context);
			if (!qmi_service_send(csd->svc, QMI_CSD_OPEN_CONTEXT_FULL, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 3: {
			if (!qmi_result_get_uint32(result, 0x11, &csd->context_handle)) {
				DBG("no context handle returned");
				return -1;
			}
			DBG("context_handle = 0x%"PRIx32, csd->context_handle);
			const struct qmi_csd_set_device_config device_config = {
					._0 = 0,
					.tx_device_id = csd->tx_device_id, .rx_device_id = csd->rx_device_id,
					.tx_sample_rate = SAMPLE_RATE, .rx_sample_rate = SAMPLE_RATE,
			};
			qmi_param_append_uint32(param, 1, csd->context_handle);
			qmi_param_append(param, 2, sizeof(device_config), &device_config);
			if (!qmi_service_send(csd->svc, QMI_CSD_SET_DEVICE_CONFIG, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 4: {
			qmi_param_append_uint32(param, 1, csd->context_handle);
			qmi_param_append_uint32(param, 2, 0);
			if (!qmi_service_send(csd->svc, QMI_CSD_ENABLE_CONTEXT, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 5: {
			const struct qmi_csd_attach_or_detach_context attach_context = {
					._0 = 0, .context_handle = csd->context_handle,
			};
			qmi_param_append_uint32(param, 1, csd->manager_handle);
			qmi_param_append(param, 2, sizeof(attach_context), &attach_context);
			if (!qmi_service_send(csd->svc, QMI_CSD_ATTACH_CONTEXT, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 6: {
			qmi_param_append_uint32(param, 1, csd->manager_handle);
			qmi_param_append_uint32(param, 2, 0);
			if (!qmi_service_send(csd->svc, QMI_CSD_START_MANAGER, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 7: {
			const struct qmi_csd_set_rx_volume rx_volume = {
					._0 = 0, .volume_index = rx_volume_index,
			};
			qmi_param_append_uint32(param, 1, csd->context_handle);
			qmi_param_append(param, 2, sizeof(rx_volume), &rx_volume);
			if (!qmi_service_send(csd->svc, QMI_CSD_SET_RX_VOLUME, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 8: {
			const struct qmi_csd_set_stream_mute stream_mute = {
					._0 = 0, .stream_num = 0, ._0_2 = 0,
			};
			qmi_param_append_uint32(param, 1, csd->stream_handle);
			qmi_param_append(param, 2, sizeof(stream_mute), &stream_mute);
			if (!qmi_service_send(csd->svc, QMI_CSD_SET_STREAM_MUTE, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 9: {
			const struct qmi_csd_set_stream_mute stream_mute = {
					._0 = 0, .stream_num = 1, ._0_2 = 0,
			};
			qmi_param_append_uint32(param, 1, csd->stream_handle);
			qmi_param_append(param, 2, sizeof(stream_mute), &stream_mute);
			if (!qmi_service_send(csd->svc, QMI_CSD_SET_STREAM_MUTE, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 10: {
			csd->status = csd_status_started;
			dbus_reply(cb_data, param, dbus_message_new_method_return);
			return 0;
		}
		default:
			return -1;
	}
}

// Stop operation
static int statemachine_for_stop(struct csd_data *csd, struct csd_cb_data *cb_data, struct qmi_result *result, struct qmi_param *param) {
	switch (cb_data->state) {
		case 1: {
			qmi_param_append_uint32(param, 1, csd->manager_handle);
			qmi_param_append_uint32(param, 2, 0);
			if (!qmi_service_send(csd->svc, QMI_CSD_STOP_MANAGER, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 2: {
			const struct qmi_csd_attach_or_detach_context detach_context = {
					._0 = 0, .context_handle = csd->context_handle,
			};
			qmi_param_append_uint32(param, 1, csd->manager_handle);
			qmi_param_append(param, 2, sizeof(detach_context), &detach_context);
			if (!qmi_service_send(csd->svc, QMI_CSD_DETACH_CONTEXT, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 3: {
			qmi_param_append_uint32(param, 1, csd->context_handle);
			qmi_param_append_uint32(param, 2, 0);
			if (!qmi_service_send(csd->svc, QMI_CSD_DISABLE_CONTEXT, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 4: {
			qmi_param_append_uint32(param, 1, csd->context_handle);
			if (!qmi_service_send(csd->svc, QMI_CSD_CLOSE_CONTEXT_FULL, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 5: {
			csd->context_handle = HANDLE_UNSET;
			struct qmi_csd_disable_devices disable_devices = {
					._2 = 2, .rx_device_id = csd->rx_device_id, .tx_device_id = csd->tx_device_id,
			};
			qmi_param_append_uint32(param, 1, csd->device_control_handle);
			qmi_param_append(param, 2, sizeof(disable_devices), &disable_devices);
			if (!qmi_service_send(csd->svc, QMI_CSD_DISABLE_DEVICES, param, statemachine, cb_data, NULL))
				return -1;
			return 0;
		}
		case 6: {
			csd->status = csd_status_inited;
			dbus_reply(cb_data, param, dbus_message_new_method_return);
			return 0;
		}
		default:
			return -1;
	}
}

// TODO mutex to prevent overlapping dbus messages? prevent multiple concurrent QMI sequences too
// TODO release resources if failure partway through. also make sure 'status' is maintained correctly in that case
// XXX endianness in QMI structs
static void statemachine(struct qmi_result *result, void *_cb_data) {
	struct csd_cb_data *cb_data = _cb_data;
	struct csd_data *csd = cb_data->csd;
	struct qmi_param *param = qmi_param_new(); // not always needed, but keeps the code simpler to assume it will

	cb_data->state++;
	DBG("operation \"%s\", state = %d", cb_data->operation, cb_data->state);

	// don't check the QMI result in the starting states, because there is none
	if (cb_data->state != 1) {
		if (qmi_result_set_error(result, NULL))
			goto fail;
	}

	int rc = cb_data->statemachine_func(csd, cb_data, result, param);
	if (rc < 0)
		goto fail;

	return;

fail:
	DBG("returning error");
	dbus_reply(cb_data, param, __ofono_error_failed);

}

static void start_statemachine(struct csd_data *csd, const char *operation, csd_statemachine_func statemachine_func) {
	struct csd_cb_data *cb_data = g_new(struct csd_cb_data, 1);
	cb_data->csd = csd;
	cb_data->operation = operation;
	cb_data->statemachine_func = statemachine_func;
	cb_data->state = 0;

	statemachine(NULL, cb_data);
}

static DBusMessage *csd_init(DBusConnection *conn, DBusMessage *msg, void *_data) {
	struct csd_data *data = _data;

	if (data->pending)
		return __ofono_error_busy(msg);
	if (data->status != csd_status_uninited)
		return __ofono_error_not_available(msg);

	data->pending = dbus_message_ref(msg);
	start_statemachine(data, "init", statemachine_for_init);

	return NULL;
}

static DBusMessage *csd_start(struct DBusConnection *conn, struct DBusMessage *msg, void *_data) {
	struct csd_data *data = _data;
	qmi_csd_device_id new_tx_id, new_rx_id;

	if (data->pending)
		return __ofono_error_busy(msg);
	if (data->status != csd_status_inited)
		return __ofono_error_not_available(msg);
	if (!dbus_message_get_args(msg, NULL, QMI_CSD_DEVICE_ID_DBUS_TYPE, &new_rx_id, QMI_CSD_DEVICE_ID_DBUS_TYPE, &new_tx_id, DBUS_TYPE_INVALID))
		return __ofono_error_invalid_args(msg);

	data->pending = dbus_message_ref(msg);
	data->rx_device_id = new_rx_id;
	data->tx_device_id = new_tx_id;
	start_statemachine(data, "start", statemachine_for_start);

	return NULL;
}

static DBusMessage *csd_stop(struct DBusConnection *conn, struct DBusMessage *msg, void *_data) {
	struct csd_data *data = _data;

	if (data->pending)
		return __ofono_error_busy(msg);
	if (data->status != csd_status_started)
		return __ofono_error_not_available(msg);

	data->pending = dbus_message_ref(msg);
	start_statemachine(data, "stop", statemachine_for_stop);

	return NULL;
}

static const struct GDBusMethodTable csd_methods[] = {
	{ GDBUS_ASYNC_METHOD("Init", NULL, NULL, csd_init) },
	{ GDBUS_ASYNC_METHOD("StartVoice", GDBUS_ARGS({ "rx_device", "u"}, {"tx_device", "u"}), NULL, csd_start) },
	{ GDBUS_ASYNC_METHOD("StopVoice", NULL, NULL, csd_stop) },
	{ }
};

static void csd_create_cb(struct qmi_service *service, void *_modem_path) {
	const char *modem_path = _modem_path;
	struct csd_data *data = g_new0(struct csd_data, 1);
	data->device_control_handle = data->stream_handle = data->manager_handle = data->context_handle = HANDLE_UNSET;
	data->svc = qmi_service_ref(service);

	if (!g_dbus_register_interface(ofono_dbus_get_connection(), modem_path,
								   CSD_INTERFACE_NAME, csd_methods,
								   NULL, NULL, data, NULL)) {
		ofono_error("Error registering dbus");
		return;
	}
}

// Called by qmimodem voicecall
void qmi_csd_register(struct qmi_device *device, const char *modem_path) {
	// TODO destroy func?
	if (!qmi_service_create(device, QMI_SERVICE_CSD, csd_create_cb, (void*)modem_path, NULL)) {
		ofono_error("Unable to request CSD service");
		return;
	}

}
