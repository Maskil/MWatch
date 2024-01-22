#ifndef RTC_H
#define RTC_H

#include "config.h"
#include "time.h"
#include <TimeLib.h>
#include <Rtc_Pcf8563.h>

#define RTC_PCF_ADDR    0x51
#define YEAR_OFFSET_PCF 2000

class RTC {
  public:
    Rtc_Pcf8563 rtc_pcf;
    bool rtc_success;

  public:
    RTC();
    void init();
    void config(); // YYYY:MM:DD:HH:MM:SS
    void clearAlarm();
    void read(tmElements_t &tm);
    void set(tmElements_t tm);

  private:
    void _PCFConfig(String datetime);
    int _getDayOfWeek(int d, int m, int y);
    String _getValue(String data, char separator, int index);
};

#endif
