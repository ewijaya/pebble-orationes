#pragma once
#include <stdbool.h>
#include <time.h>
time_t reminder_schedule_next_noon(time_t now);
bool reminder_schedule_is_timely(time_t now, time_t scheduled_time);
