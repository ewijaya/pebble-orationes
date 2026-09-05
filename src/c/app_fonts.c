#include "app_fonts.h"

// Stable handles prevent the text-layout cache from confusing a newly loaded
// font with another size previously allocated at the same address. Share these
// handles between the reader and appearance preview for the whole app lifetime.
static GFont s_fonts[3];
static GFont custom_font(uint8_t index) {
  static const uint32_t resources[] = {
      RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_34,
      RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_OBLIQUE_28,
      RESOURCE_ID_FONT_DEJAVU_SANS_CONDENSED_BOLD_OBLIQUE_34,
  };
  if (!s_fonts[index])
    s_fonts[index] =
        fonts_load_custom_font(resource_get_handle(resources[index]));
  return s_fonts[index];
}
GFont app_fonts_body(bool extra_large) {
  GFont font = extra_large ? custom_font(0) : NULL;
  return font ? font : fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);
}
GFont app_fonts_italic(bool extra_large) {
  return custom_font(extra_large ? 2 : 1);
}
void app_fonts_deinit(void) {
  for (uint8_t i = 0; i < 3; ++i) {
    if (s_fonts[i])
      fonts_unload_custom_font(s_fonts[i]);
    s_fonts[i] = NULL;
  }
}
