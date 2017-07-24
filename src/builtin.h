extern struct ofono_plugin_desc __ofono_builtin_rildev;
extern struct ofono_plugin_desc __ofono_builtin_ril;
extern struct ofono_plugin_desc __ofono_builtin_infineon;
extern struct ofono_plugin_desc __ofono_builtin_ril_intel;
extern struct ofono_plugin_desc __ofono_builtin_rilmodem;
extern struct ofono_plugin_desc __ofono_builtin_smart_messaging;
extern struct ofono_plugin_desc __ofono_builtin_push_notification;
extern struct ofono_plugin_desc __ofono_builtin_allowed_apns;

static struct ofono_plugin_desc *__ofono_builtin[] = {
  &__ofono_builtin_rildev,
  &__ofono_builtin_ril,
  &__ofono_builtin_infineon,
  &__ofono_builtin_ril_intel,
  &__ofono_builtin_rilmodem,
  &__ofono_builtin_smart_messaging,
  &__ofono_builtin_push_notification,
  &__ofono_builtin_allowed_apns,
  NULL
};
