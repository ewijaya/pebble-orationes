#pragma once

#include <stdbool.h>
#include <time.h>

bool liturgical_calendar_is_eastertide(time_t timestamp);
bool liturgical_calendar_is_eastertide_date(int year, int month, int day);
