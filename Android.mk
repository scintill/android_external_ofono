LOCAL_PATH := $(call my-dir)

# reference commands for generating these values are fish shell

# bash ~/opt/android-ndk-r10d/build/tools/make-standalone-toolchain.sh --arch=arm --install-dir=./toolchain --platform=android-21

# env PATH=(sh -c 'echo $PATH'):$PWD/toolchain/bin ./configure \
#   --host=arm-linux-androideabi \
#   --disable-udev \
#   --disable-atmodem \
#   --disable-cdmamodem \
#   --disable-phonesim \
#   --disable-isimodem \
#   --disable-qmimodem \
#   --disable-bluetooth \
#   --disable-upower \
#   --disable-datafiles \
#   --disable-provision \
#   --prefix=/system \
#   --localstatedir=/data/misc/radio/ofono

# src/genbuiltin (make -f (printf 'printvars:\n\t @echo $(builtin_modules)' | psub) -f Makefile) > src/builtin.h

include $(CLEAR_VARS)
	LOCAL_MODULE := ofonod

	# make -f (printf 'printvars:\n\t @echo $(COMPILE)' | psub) -f Makefile

	LOCAL_CFLAGS := -DOFONO_PLUGIN_BUILTIN -DPLUGINDIR=\"/system/lib/ofono/plugins\" -Wall -O2 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -DHAVE_CONFIG_H

	LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/include $(LOCAL_PATH)/src $(LOCAL_PATH)/gdbus $(LOCAL_PATH)/gisi $(LOCAL_PATH)/gatchat $(LOCAL_PATH)/btio $(LOCAL_PATH)/gril

	# make -f (printf 'printvars:\n\t @echo $(src_ofonod_OBJECTS) $(gdbus_libgdbus_internal_la_OBJECTS)' | psub) -f Makefile | sed 's/\.l\?o/.c/g' | xargs -n 1 echo | sort | xargs echo | fmt | sed 's/$/ \\\\/'
	LOCAL_SRC_FILES := \
		drivers/rilmodem/call-barring.c drivers/rilmodem/call-forwarding.c \
		drivers/rilmodem/call-settings.c drivers/rilmodem/call-volume.c \
		drivers/rilmodem/cbs.c drivers/rilmodem/devinfo.c drivers/rilmodem/gprs.c \
		drivers/rilmodem/gprs-context.c drivers/rilmodem/lte.c \
		drivers/rilmodem/netmon.c drivers/rilmodem/network-registration.c \
		drivers/rilmodem/radio-settings.c drivers/rilmodem/rilmodem.c \
		drivers/rilmodem/rilutil.c drivers/rilmodem/sim.c \
		drivers/rilmodem/sms.c drivers/rilmodem/stk.c drivers/rilmodem/ussd.c \
		drivers/rilmodem/voicecall.c gatchat/crc-ccitt.c gatchat/gatchat.c \
		gatchat/gathdlc.c gatchat/gatio.c gatchat/gatmux.c gatchat/gatppp.c \
		gatchat/gatrawip.c gatchat/gatresult.c gatchat/gatserver.c \
		gatchat/gatsyntax.c gatchat/gattty.c gatchat/gatutil.c \
		gatchat/gsm0710.c gatchat/ppp_auth.c gatchat/ppp_cp.c \
		gatchat/ppp_ipcp.c gatchat/ppp_ipv6cp.c gatchat/ppp_lcp.c \
		gatchat/ppp_net.c gatchat/ringbuffer.c gdbus/client.c gdbus/mainloop.c \
		gdbus/object.c gdbus/polkit.c gdbus/watch.c gril/gril.c \
		gril/grilio.c gril/grilutil.c gril/parcel.c plugins/allowed-apns.c \
		plugins/infineon.c plugins/push-notification.c plugins/ril.c \
		plugins/rildev.c plugins/ril_intel.c plugins/smart-messaging.c \
		src/audio-settings.c src/call-barring.c src/call-forwarding.c \
		src/call-meter.c src/call-settings.c src/call-volume.c src/cbs.c \
		src/cdma-connman.c src/cdma-netreg.c src/cdma-provision.c src/cdma-sms.c \
		src/cdma-smsutil.c src/cdma-voicecall.c src/common.c src/ctm.c src/dbus.c \
		src/emulator.c src/gnssagent.c src/gnss.c src/gprs.c src/gprs-provision.c \
		src/handsfree-audio.c src/handsfree.c src/history.c src/idmap.c \
		src/location-reporting.c src/log.c src/lte.c src/main.c src/manager.c \
		src/message.c src/message-waiting.c src/modem.c src/netmon.c src/nettime.c \
		src/network.c src/phonebook.c src/plugin.c src/private-network.c \
		src/radio-settings.c src/sim-auth.c src/sim.c src/simfs.c src/simutil.c \
		src/siri.c src/smsagent.c src/sms.c src/smsutil.c src/stkagent.c src/stk.c \
		src/stkutil.c src/storage.c src/ussd.c src/util.c src/voicecall.c \
		src/watch.c \
		\
		android/glibc.c

	LOCAL_SHARED_LIBRARIES := libglib libdbus libdl

	LOCAL_C_INCLUDES += external/glib external/glib/glib external/dbus

	LOCAL_LDFLAGS := -Wl,--wrap=bsearch
include $(BUILD_EXECUTABLE)

## symlink .h files so the .c files can #include <ofono/X.h> . This is done in Makefile.am
# make -f (printf 'printvars:\n\t @echo $(BUILT_SOURCES)' | psub) -f Makefile
$(addprefix $(LOCAL_PATH)/, $(LOCAL_SRC_FILES)): $(addprefix $(LOCAL_PATH)/, \
		include/ofono/log.h include/ofono/plugin.h include/ofono/history.h \
		include/ofono/dbus.h include/ofono/modem.h include/ofono/types.h \
		include/ofono/call-barring.h include/ofono/call-forwarding.h \
		include/ofono/call-meter.h include/ofono/call-settings.h \
		include/ofono/phonebook.h include/ofono/ussd.h include/ofono/sms.h \
		include/ofono/sim.h include/ofono/message-waiting.h \
		include/ofono/netreg.h include/ofono/voicecall.h include/ofono/devinfo.h \
		include/ofono/cbs.h include/ofono/call-volume.h include/ofono/gprs.h \
		include/ofono/gprs-context.h include/ofono/radio-settings.h \
		include/ofono/stk.h include/ofono/audio-settings.h include/ofono/nettime.h \
		include/ofono/ctm.h include/ofono/cdma-voicecall.h \
		include/ofono/cdma-sms.h include/ofono/sim-auth.h \
		include/ofono/gprs-provision.h include/ofono/emulator.h \
		include/ofono/location-reporting.h include/ofono/cdma-connman.h \
		include/ofono/gnss.h include/ofono/private-network.h \
		include/ofono/cdma-netreg.h include/ofono/cdma-provision.h \
		include/ofono/handsfree.h include/ofono/handsfree-audio.h \
		include/ofono/siri.h include/ofono/netmon.h include/ofono/lte.h \
		include/ofono/version.h \
		)

$(LOCAL_PATH)/include/ofono/%.h: $(LOCAL_PATH)/include/%.h
	$(hide) mkdir -p $(shell dirname $@)
	$(hide) ln -s $(realpath $<) $@
