#include "reminder_schedule.h"
#include <pebble.h>
#include <stdint.h>

enum { MAX_MISSED_REMINDER_SECONDS = 15 * 60, SCHEDULE_TOLERANCE_SECONDS = 60 };

time_t reminder_schedule_next_noon(time_t now) {
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

bool reminder_schedule_is_timely(time_t now, time_t scheduled_time) {
  if (scheduled_time <= 0) {
    return true;
  }

  const int32_t delay = (int32_t)(now - scheduled_time);
  return delay >= -SCHEDULE_TOLERANCE_SECONDS &&
         delay <= MAX_MISSED_REMINDER_SECONDS;
}
