#pragma once
typedef enum {
  APPEARANCE_PICKER_SIZE,
  APPEARANCE_PICKER_APPEARANCE,
  APPEARANCE_PICKER_ACCENT,
  APPEARANCE_PICKER_NAVIGATION
} AppearancePickerKind;
void appearance_picker_init(void);
void appearance_picker_deinit(void);
void appearance_picker_show(AppearancePickerKind kind, void (*saved)(void));
void appearance_picker_refresh(void);
