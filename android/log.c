/*
 *
 *  oFono - Open Source Telephony
 *
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

/*
 * Android's libc log functions are almost exactly what I want, but
 * they log to the main log instead of the radio log.
 * We use a linker flag to intercept calls to syslog to here, then we'll
 * send them out to Android's liblog into the radio log.
 */

#define LOG_TAG "ofonod"

#include <log/log.h>
#include <syslog.h>

void __wrap_vsyslog(int priority, const char *format, va_list ap) {
	/*
	 * I'd use a __android_log_buf_vprint (printf to a specified buffer with a va_list) if it existed.
	 * The following is what its cousin __android_log_vprint does.
	 */
	char buf[1024];
	vsnprintf(buf, sizeof(buf), format, ap);

	int android_priority;
	// these are the priority mappings that bionic's syslog() uses
	if (priority <= LOG_ERR) {
		android_priority = ANDROID_LOG_ERROR;
	} else if (priority == LOG_WARNING) {
		android_priority = ANDROID_LOG_WARN;
	} else if (priority <= LOG_INFO) {
		android_priority = ANDROID_LOG_INFO;
	} else {
		android_priority = ANDROID_LOG_DEBUG;
	}

	__android_log_buf_write(LOG_ID_RADIO, android_priority, LOG_TAG, buf);
}

void __wrap_syslog(int priority, const char *format, ...) {
	va_list ap;
	va_start(ap, format);

	__wrap_vsyslog(priority, format, ap);

	va_end(ap);
}
