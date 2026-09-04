#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  APP_TEXT_SIZE_LARGE,
  APP_TEXT_SIZE_EXTRA_LARGE,
  APP_TEXT_SIZE_COUNT,
} AppTextSize;

void app_settings_init(void);
AppTextSize app_settings_get_text_size(void);
bool app_settings_set_text_size(AppTextSize text_size);
const char *app_settings_text_size_label(AppTextSize text_size);
