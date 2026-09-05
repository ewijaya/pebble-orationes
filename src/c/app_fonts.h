#pragma once
#include <pebble.h>

GFont app_fonts_body(bool extra_large);
GFont app_fonts_italic(bool extra_large);
// Call after every window has been destroyed.
void app_fonts_deinit(void);
