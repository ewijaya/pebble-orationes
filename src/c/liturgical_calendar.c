#include "liturgical_calendar.h"

#include <pebble.h>

static void gregorian_easter_date(int year, int *month, int *day) {
  const int a = year % 19;
  const int b = year / 100;
  const int c = year % 100;
  const int d = b / 4;
  const int e = b % 4;
  const int f = (b + 8) / 25;
  const int g = (b - f + 1) / 3;
  const int h = (19 * a + b - d - g + 15) % 30;
  const int i = c / 4;
  const int k = c % 4;
  const int l = (32 + 2 * e + 2 * i - h - k) % 7;
  const int m = (a + 11 * h + 22 * l) / 451;
  const int encoded_date = h + l - 7 * m + 114;

  *month = encoded_date / 31;
  *day = encoded_date % 31 + 1;
}

static bool is_leap_year(int year) {
  return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

static int day_of_year(int year, int month, int day) {
  static const int s_days_before_month[] = {
      0,   31,  59,  90,  120, 151,
      181, 212, 243, 273, 304, 334,
  };

  int result = s_days_before_month[month - 1] + day;
  if (month > 2 && is_leap_year(year)) {
    result += 1;
  }
  return result;
}

bool liturgical_calendar_is_eastertide_date(int year, int month, int day) {
  if (month < 1 || month > 12 || day < 1 || day > 31) {
    return false;
  }

  int easter_month;
  int easter_day;
  gregorian_easter_date(year, &easter_month, &easter_day);

  const int current_day = day_of_year(year, month, day);
  const int easter_sunday =
      day_of_year(year, easter_month, easter_day);
  const int pentecost_sunday = easter_sunday + 49;

  return current_day >= easter_sunday && current_day <= pentecost_sunday;
}

bool liturgical_calendar_is_eastertide(time_t timestamp) {
  const struct tm *local_time = localtime(&timestamp);
  if (!local_time) {
    return false;
  }

  return liturgical_calendar_is_eastertide_date(
      local_time->tm_year + 1900, local_time->tm_mon + 1,
      local_time->tm_mday);
}
