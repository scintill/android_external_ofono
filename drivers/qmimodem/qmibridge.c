
#include <libqmi-glib.h>

#include <string.h>
#include <stdio.h>

#include <ofono/log.h>

#include <glib.h>

static void ask_qmi(const char *prefix, void *data, size_t len)
{
	/* from osmo-qcdiag */
	GByteArray *buffer;
	GError *error = NULL;
	QmiMessage *message;
	gchar *printable;

	buffer = g_byte_array_sized_new(len);
	g_byte_array_append(buffer, data, len);

	message = qmi_message_new_from_raw(buffer, &error);
	if (!message) {
		fprintf(stderr, "qmi_message_new_from_raw() returned NULL\n");
		return;
	}

	printable = qmi_message_get_printable(message, "QMI ");
	DBG("%s: %s", prefix, printable);
	g_free(printable);
}
void qmibridge_decode_read(void *data, size_t len)
{
	ask_qmi("READ", data, len);
}

void qmibridge_decode_req(void *data, size_t len)
{
	ask_qmi("_REQ", data, len);
}
