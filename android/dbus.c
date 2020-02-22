/*
 * Based on dbus code. This function is not in the version
 * of dbus that we are using on Android. -- Joey Hewitt
 *
 * Copyright (C) 2005 Red Hat, Inc.
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

// some white lies
#define DBUS_COMPILATION
#include <dbus/dbus-internals.h>

/**
 * Determine wether the given character is valid as a second or later
 * character in a name
 */
#define VALID_NAME_CHARACTER(c)                 \
  ( ((c) >= '0' && (c) <= '9') ||               \
    ((c) >= 'A' && (c) <= 'Z') ||               \
    ((c) >= 'a' && (c) <= 'z') ||               \
    ((c) == '_') )

dbus_bool_t dbus_validate_path(const unsigned char *s, DBusError *error) {
	const unsigned char *end;
	const unsigned char *last_slash;
	int len = strlen(s);

	if (len == 0) {
		dbus_set_error(error, DBUS_ERROR_INVALID_ARGS,
					   "Object path was not valid: '%s'", s);
		return FALSE;
	}

	end = s + len;

	if (*s != '/') {
		dbus_set_error (error, DBUS_ERROR_INVALID_ARGS,
						"Object path was not valid: '%s'", s);
		return FALSE;
	}
	last_slash = s;
	++s;

	while (s != end)
	{
		if (*s == '/')
		{
			if ((s - last_slash) < 2) {
				dbus_set_error (error, DBUS_ERROR_INVALID_ARGS,
								"Object path was not valid: '%s'", s);
				return FALSE; /* no empty path components allowed */
			}

			last_slash = s;
		}
		else
		{
			if (_DBUS_UNLIKELY (!VALID_NAME_CHARACTER (*s))) {
				dbus_set_error (error, DBUS_ERROR_INVALID_ARGS,
								"Object path was not valid: '%s'", s);
				return FALSE;
			}
		}

		++s;
	}

	if ((end - last_slash) < 2 &&
		len > 1) {
		dbus_set_error (error, DBUS_ERROR_INVALID_ARGS,
						"Object path was not valid: '%s'", s);
		return FALSE; /* trailing slash not allowed unless the string is "/" */
	}

	return TRUE;
}
