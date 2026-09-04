#include "noon_reminder.h"

#include <pebble.h>

#include "app_settings.h"
#include "app_theme.h"
#include "liturgical_calendar.h"
#include "prayer_screen.h"
#include "prayers.h"

enum {
  PERSIST_KEY_WAKEUP_ID = 10,
  PERSIST_KEY_WAKEUP_TIME = 11,
  NOON_REMINDER_COOKIE = 0x4e4f4f4e,
  MAX_MISSED_REMINDER_SECONDS = 15 * 60,
  SCHEDULE_TOLERANCE_SECONDS = 60,
  HORIZONTAL_MARGIN = 8,
};

static Window *s_window;
static TextLayer *s_title_layer;
static TextLayer *s_prayer_layer;
static TextLayer *s_hint_layer;
static const Prayer *s_prayer;
static AppTimer *s_dismiss_timer;
static WakeupId s_wakeup_id = -1;
static time_t s_scheduled_time;

static void stop_dismiss_timer(void) {
  if (s_dismiss_timer) {
    app_timer_cancel(s_dismiss_timer);
    s_dismiss_timer = NULL;
  }
}

static void clear_schedule_record(void) {
  if (persist_exists(PERSIST_KEY_WAKEUP_ID)) {
    persist_delete(PERSIST_KEY_WAKEUP_ID);
  }
  if (persist_exists(PERSIST_KEY_WAKEUP_TIME)) {
    persist_delete(PERSIST_KEY_WAKEUP_TIME);
  }

  s_wakeup_id = -1;
  s_scheduled_time = 0;
}

static void load_schedule_record(void) {
  s_wakeup_id = -1;
  s_scheduled_time = 0;

  if (persist_exists(PERSIST_KEY_WAKEUP_ID) &&
      persist_exists(PERSIST_KEY_WAKEUP_TIME)) {
    s_wakeup_id = persist_read_int(PERSIST_KEY_WAKEUP_ID);
    s_scheduled_time = persist_read_int(PERSIST_KEY_WAKEUP_TIME);
  }
}

static bool save_schedule_record(WakeupId wakeup_id, time_t timestamp) {
  if (persist_write_int(PERSIST_KEY_WAKEUP_ID, wakeup_id) < 0 ||
      persist_write_int(PERSIST_KEY_WAKEUP_TIME, (int32_t)timestamp) < 0) {
    wakeup_cancel(wakeup_id);
    clear_schedule_record();
    return false;
  }

  s_wakeup_id = wakeup_id;
  s_scheduled_time = timestamp;
  return true;
}

static void cancel_scheduled_wakeup(void) {
  // Persistence can be cleared by an uninstall while firmware retains the
  // scheduled event. Noon prayer is Orationes' only wakeup, so cancel every
  // wakeup owned by the app to recover safely from an orphaned schedule.
  wakeup_cancel_all();
  clear_schedule_record();
}

static time_t get_next_local_noon(time_t now) {
  const struct tm *current_time = localtime(&now);
  if (!current_time) {
    return 0;
  }

  struct tm noon = *current_time;
  noon.tm_hour = 12;
  noon.tm_min = 0;
  noon.tm_sec = 0;
  noon.tm_isdst = -1;

  time_t next_noon = mktime(&noon);
  if (next_noon <= now) {
    noon.tm_mday += 1;
    noon.tm_isdst = -1;
    next_noon = mktime(&noon);
  }
  return next_noon;
}

static bool schedule_wakeup(time_t timestamp) {
  if (timestamp <= 0) {
    return false;
  }

  const WakeupId wakeup_id =
      wakeup_schedule(timestamp, NOON_REMINDER_COOKIE, true);
  if (wakeup_id < 0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Noon wakeup schedule failed: %ld",
            (long)wakeup_id);
    return false;
  }

  APP_LOG(APP_LOG_LEVEL_INFO, "Noon wakeup %ld scheduled for %ld",
          (long)wakeup_id, (long)timestamp);

  return save_schedule_record(wakeup_id, timestamp);
}

static bool sync_scheduled_wakeup(void) {
  const time_t now = time(NULL);
  const time_t expected_time = get_next_local_noon(now);
  time_t actual_time = 0;

  if (s_wakeup_id >= 0 && wakeup_query(s_wakeup_id, &actual_time)) {
    const int32_t difference = (int32_t)(actual_time - expected_time);
    if (difference >= -SCHEDULE_TOLERANCE_SECONDS &&
        difference <= SCHEDULE_TOLERANCE_SECONDS) {
      s_scheduled_time = actual_time;
      return true;
    }
  }

  cancel_scheduled_wakeup();
  return schedule_wakeup(expected_time);
}

static void choose_current_prayer(time_t timestamp) {
  const PrayerId prayer_id =
      liturgical_calendar_is_eastertide(timestamp) ? PRAYER_ID_REGINA_CAELI
                                                   : PRAYER_ID_ANGELUS;
  s_prayer = prayers_get_by_id(prayer_id);

  if (s_prayer_layer && s_prayer) {
    text_layer_set_text(s_prayer_layer, s_prayer->name);
  }
}

static bool reminder_is_timely(time_t now, time_t scheduled_time) {
  if (scheduled_time <= 0) {
    return true;
  }

  const int32_t delay = (int32_t)(now - scheduled_time);
  return delay >= -SCHEDULE_TOLERANCE_SECONDS &&
         delay <= MAX_MISSED_REMINDER_SECONDS;
}

static void open_prayer_handler(ClickRecognizerRef recognizer, void *context) {
  (void)recognizer;
  (void)context;

  if (!s_prayer) {
    return;
  }

  const PrayerTranslation *translation =
      prayer_get_translation(s_prayer, s_prayer->default_language);
  if (!translation) {
    return;
  }

  prayer_screen_show(s_prayer->name, translation->text);
  window_stack_remove(s_window, false);
}

static void click_config_provider(void *context) {
  (void)context;
  window_single_click_subscribe(BUTTON_ID_SELECT, open_prayer_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  const GRect bounds = layer_get_bounds(window_layer);
  const int16_t text_width = bounds.size.w - 2 * HORIZONTAL_MARGIN;

  window_set_background_color(window, app_theme_background_color());

  s_title_layer = text_layer_create(
      GRect(0, 0, bounds.size.w, 44));
  text_layer_set_background_color(s_title_layer,
                                  app_theme_title_background_color());
  text_layer_set_text_color(s_title_layer,
                            app_theme_title_foreground_color());
  text_layer_set_font(
      s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_text(s_title_layer, "Noon Prayer");
  layer_add_child(window_layer, text_layer_get_layer(s_title_layer));

  s_prayer_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, 72, text_width, 70));
  text_layer_set_background_color(s_prayer_layer, GColorClear);
  text_layer_set_text_color(s_prayer_layer, app_theme_foreground_color());
  text_layer_set_font(
      s_prayer_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(s_prayer_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_prayer_layer, GTextOverflowModeWordWrap);
  text_layer_set_text(s_prayer_layer, s_prayer ? s_prayer->name : "");
  layer_add_child(window_layer, text_layer_get_layer(s_prayer_layer));

  s_hint_layer = text_layer_create(
      GRect(HORIZONTAL_MARGIN, 154, text_width, 60));
  text_layer_set_background_color(s_hint_layer, GColorClear);
  text_layer_set_text_color(s_hint_layer, app_theme_foreground_color());
  text_layer_set_font(
      s_hint_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_hint_layer, GTextAlignmentCenter);
  text_layer_set_text(s_hint_layer, "SELECT: Open\nBACK: Dismiss");
  layer_add_child(window_layer, text_layer_get_layer(s_hint_layer));
}

static void window_unload(Window *window) {
  stop_dismiss_timer();

  text_layer_destroy(s_hint_layer);
  s_hint_layer = NULL;

  text_layer_destroy(s_prayer_layer);
  s_prayer_layer = NULL;

  text_layer_destroy(s_title_layer);
  s_title_layer = NULL;
}

static void dismiss_timer_handler(void *context) {
  (void)context;
  s_dismiss_timer = NULL;
  window_stack_remove(s_window, true);
}

void noon_reminder_show(void) {
  if (!s_prayer || quiet_time_is_active()) {
    return;
  }

  vibes_double_pulse();
  if (!window_stack_contains_window(s_window)) {
    window_stack_push(s_window, true);
  }

  stop_dismiss_timer();
  s_dismiss_timer = app_timer_register(
      app_settings_get_noon_reminder_duration_seconds() * 1000,
      dismiss_timer_handler, NULL);
}

static void wakeup_handler(WakeupId wakeup_id, int32_t cookie) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Noon wakeup %ld received", (long)wakeup_id);

  if (cookie != NOON_REMINDER_COOKIE) {
    return;
  }

  (void)wakeup_id;
  clear_schedule_record();

  if (!app_settings_get_noon_reminder_enabled()) {
    return;
  }

  sync_scheduled_wakeup();
  if (quiet_time_is_active()) {
    return;
  }

  choose_current_prayer(time(NULL));
  noon_reminder_show();
}

bool noon_reminder_init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_click_config_provider(s_window, click_config_provider);
  window_set_window_handlers(s_window, (WindowHandlers){
      .load = window_load,
      .unload = window_unload,
  });

  wakeup_service_subscribe(wakeup_handler);
  load_schedule_record();

  WakeupId launch_wakeup_id;
  int32_t launch_cookie;
  const bool launched_by_reminder =
      wakeup_get_launch_event(&launch_wakeup_id, &launch_cookie) &&
      launch_cookie == NOON_REMINDER_COOKIE;
  const time_t prior_scheduled_time = s_scheduled_time;
  const time_t now = time(NULL);

  if (app_settings_get_noon_reminder_enabled()) {
    sync_scheduled_wakeup();
  } else {
    cancel_scheduled_wakeup();
  }

  if (!launched_by_reminder ||
      !app_settings_get_noon_reminder_enabled() ||
      !reminder_is_timely(now, prior_scheduled_time) ||
      quiet_time_is_active()) {
    return false;
  }

  choose_current_prayer(now);
  return s_prayer != NULL;
}

void noon_reminder_deinit(void) {
  window_destroy(s_window);
  s_window = NULL;
}

bool noon_reminder_set_enabled(bool enabled) {
  if (!app_settings_set_noon_reminder_enabled(enabled)) {
    return false;
  }

  if (!enabled) {
    cancel_scheduled_wakeup();
    return true;
  }

  if (sync_scheduled_wakeup()) {
    return true;
  }

  app_settings_set_noon_reminder_enabled(false);
  cancel_scheduled_wakeup();
  return false;
}
