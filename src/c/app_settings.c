#include "app_settings.h"

#include <pebble.h>

enum {
  PERSIST_KEY_TEXT_SIZE = 1,
};

static AppTextSize s_text_size = APP_TEXT_SIZE_LARGE;

static bool text_size_is_valid(int32_t value) {
  return value >= APP_TEXT_SIZE_LARGE && value < APP_TEXT_SIZE_COUNT;
}

void app_settings_init(void) {
  s_text_size = APP_TEXT_SIZE_LARGE;

  if (persist_exists(PERSIST_KEY_TEXT_SIZE)) {
    const int32_t stored_text_size = persist_read_int(PERSIST_KEY_TEXT_SIZE);
    if (text_size_is_valid(stored_text_size)) {
      s_text_size = (AppTextSize)stored_text_size;
    }
  }
}

AppTextSize app_settings_get_text_size(void) {
  return s_text_size;
}

bool app_settings_set_text_size(AppTextSize text_size) {
  if (!text_size_is_valid(text_size)) {
    return false;
  }

  if (persist_write_int(PERSIST_KEY_TEXT_SIZE, text_size) < 0) {
    return false;
  }

  s_text_size = text_size;
  return true;
}

const char *app_settings_text_size_label(AppTextSize text_size) {
  switch (text_size) {
    case APP_TEXT_SIZE_LARGE:
      return "Large";
    case APP_TEXT_SIZE_EXTRA_LARGE:
      return "Extra Large";
    case APP_TEXT_SIZE_COUNT:
      break;
  }

  return "Large";
}
