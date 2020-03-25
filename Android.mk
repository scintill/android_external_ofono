LOCAL_PATH := $(call my-dir)

# Reference commands for generating these values are for fish shell and Ubuntu-like system.
# Some of the commands in the block below add their outputs to git.

# bash ~/opt/android-ndk-r11c/build/tools/make-standalone-toolchain.sh --arch=arm --install-dir=./toolchain --platform=android-21
# ./bootstrap
# sudo apt-get install libdbus-1-dev # install deps on build machine, to get configure checks to pass
# env PATH=(sh -c 'echo $PATH'):$PWD/toolchain/bin ./configure \
#   --enable-silent-rules \
#   --host=arm-linux-androideabi \
#   --enable-pie \
#   --with-pic \
#   --disable-udev \
#   --disable-atmodem \
#   --disable-cdmamodem \
#   --disable-phonesim \
#   --disable-isimodem \
#   --disable-rilmodem \
#   --disable-mbimmodem \
#   --disable-bluetooth \
#   --disable-provision \
#   --disable-upower \
#   --disable-datafiles \
#   --prefix=/system \
#   --localstatedir=/data/misc/radio/ofono
# make V=1 ell/internal ell/ell.h # modify commands to use relative paths, and re-run
# git add -f config.h include/version.h ell

include $(CLEAR_VARS)
	LOCAL_MODULE := ofonod

	# make -f (printf 'printvars:\n\t @echo $(COMPILE)' | psub) -f Makefile
	LOCAL_CFLAGS := -DHAVE_CONFIG_H -DOFONO_PLUGIN_BUILTIN -DPLUGINDIR=\"/system/lib/ofono/plugins\" -Wall -O2 -fsigned-char -fno-exceptions -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -fPIE
	LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/include $(LOCAL_PATH)/src $(LOCAL_PATH)/src $(LOCAL_PATH)/gdbus $(LOCAL_PATH)/gisi $(LOCAL_PATH)/gatchat $(LOCAL_PATH)/btio $(LOCAL_PATH)/gril

	# make -f (printf 'printvars:\n\t @echo $(src_ofonod_OBJECTS) $(gdbus_libgdbus_internal_la_OBJECTS) $(ell_libell_internal_la_OBJECTS)' | psub) -f Makefile | sed 's/\.l\?o/.c/g' | xargs -n 1 echo | sort -u | xargs echo | fmt | sed 's/$/ \\\\/'
	LOCAL_SRC_FILES := \
        drivers/qmimodem/devinfo.c drivers/qmimodem/gprs.c \
        drivers/qmimodem/gprs-context.c drivers/qmimodem/location-reporting.c \
        drivers/qmimodem/lte.c drivers/qmimodem/nas.c drivers/qmimodem/netmon.c \
        drivers/qmimodem/network-registration.c drivers/qmimodem/qmi.c \
        drivers/qmimodem/qmimodem.c drivers/qmimodem/radio-settings.c \
        drivers/qmimodem/sim.c drivers/qmimodem/sim-legacy.c \
        drivers/qmimodem/sms.c drivers/qmimodem/ussd.c \
        drivers/qmimodem/voicecall.c ell/checksum.c ell/file.c \
        ell/gpio.c ell/idle.c ell/io.c ell/log.c ell/main.c ell/queue.c \
        ell/random.c ell/signal.c ell/string.c ell/strv.c ell/test.c \
        ell/timeout.c ell/uintset.c ell/utf8.c ell/util.c ell/uuid.c \
        gatchat/crc-ccitt.c gatchat/gatchat.c gatchat/gathdlc.c \
        gatchat/gatio.c gatchat/gatmux.c gatchat/gatppp.c gatchat/gatrawip.c \
        gatchat/gatresult.c gatchat/gatserver.c gatchat/gatsyntax.c \
        gatchat/gattty.c gatchat/gatutil.c gatchat/gsm0710.c gatchat/ppp_auth.c \
        gatchat/ppp_cp.c gatchat/ppp_ipcp.c gatchat/ppp_ipv6cp.c gatchat/ppp_lcp.c \
        gatchat/ppp_net.c gatchat/ringbuffer.c gdbus/client.c gdbus/mainloop.c \
        gdbus/object.c gdbus/polkit.c gdbus/watch.c plugins/allowed-apns.c \
        plugins/gobi.c plugins/push-notification.c plugins/smart-messaging.c \
        src/audio-settings.c src/call-barring.c src/call-forwarding.c \
        src/call-meter.c src/call-settings.c src/call-volume.c src/cbs.c \
        src/cdma-connman.c src/cdma-netreg.c src/cdma-provision.c src/cdma-sms.c \
        src/cdma-smsutil.c src/cdma-voicecall.c src/common.c src/ctm.c src/dbus.c \
        src/emulator.c src/gnssagent.c src/gnss.c src/gprs.c src/gprs-provision.c \
        src/handsfree-audio.c src/handsfree.c src/history.c src/ims.c \
        src/location-reporting.c src/log.c src/lte.c src/main.c src/manager.c \
        src/message.c src/message-waiting.c src/modem.c src/netmonagent.c \
        src/netmon.c src/nettime.c src/network.c src/phonebook.c src/plugin.c \
        src/private-network.c src/radio-settings.c src/sim-auth.c src/sim.c \
        src/simfs.c src/simutil.c src/siri.c src/smsagent.c src/sms.c \
        src/smsutil.c src/stkagent.c src/stk.c src/stkutil.c src/storage.c \
        src/ussd.c src/util.c src/voicecall.c src/watch.c \
        drivers/qmimodem/voice_generated.c drivers/qmimodem/voice.c drivers/common/call_list.c \
        drivers/qmimodem/csd.c

	# Android-specific parts that are not in the upstream makefiles
	LOCAL_SRC_FILES += \
		android/glibc.c android/log.c android/dbus.c plugins/gobidev.c

	LOCAL_LDFLAGS := \
		-Wl,--wrap=bsearch \
		-Wl,--wrap=vsyslog -Wl,--wrap=syslog \

    # make -f (printf 'printvars:\n\t @echo $(builtin_modules)' | psub) -f Makefile
	OFONO_PLUGINS := qmimodem gobi gobidev smart_messaging push_notification allowed_apns

    # External deps
	LOCAL_SHARED_LIBRARIES := libglib libdbus libdl liblog

	LOCAL_C_INCLUDES += external/glib external/glib/glib external/dbus

include $(BUILD_EXECUTABLE)

## XXX how nasty are these hacks of making .c files be build targets?
## XXX plugin changes don't properly take effect (have to manually delete the builtin.h file), but I guess that'll be rare...

## symlink .h files so the .c files can #include <ofono/X.h> . This is done in Makefile.am
# make -f (printf 'printvars:\n\t @echo $(BUILT_SOURCES)' | psub) -f Makefile | fmt | sed 's/$/ \\\\/'
# then remove ell/ paths
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
        include/ofono/ims.h include/ofono/storage.h include/ofono/version.h \
        src/builtin.h \
		)

$(LOCAL_PATH)/include/ofono/%.h: $(LOCAL_PATH)/include/%.h
	$(hide) mkdir -p $(shell dirname $@)
	$(hide) ln -s $(realpath $<) $@

$(LOCAL_PATH)/src/plugin.c: $(LOCAL_PATH)/src/builtin.h

$(LOCAL_PATH)/src/builtin.h: $(LOCAL_PATH)/src/genbuiltin
	$(hide) $< $(OFONO_PLUGINS) > $@
