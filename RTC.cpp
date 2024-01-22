#include "RTC.h"

RTC::RTC() : rtc_pcf() {}

void RTC::init()
{
  Wire.beginTransmission(RTC_PCF_ADDR);
  if(Wire.endTransmission()==0)
    rtc_success = true;
}

void RTC::config()
{ // String datetime format is YYYY:MM:DD:HH:MM:SS
  clearAlarm();
}

void RTC::clearAlarm()
{
  int nextAlarmMinute = 0;
  rtc_pcf.clearAlarm();
  nextAlarmMinute = rtc_pcf.getMinute();
  nextAlarmMinute = (nextAlarmMinute == 59)?0:(nextAlarmMinute + 1);
  rtc_pcf.setAlarm(nextAlarmMinute, 99, 99, 99);
}

void RTC::read(tmElements_t &tm)
{
  rtc_pcf.getDate();
  tm.Year  = y2kYearToTm(rtc_pcf.getYear());
  tm.Month = rtc_pcf.getMonth();
  tm.Day   = rtc_pcf.getDay();
  tm.Wday = rtc_pcf.getWeekday() + 1;
  tm.Hour   = rtc_pcf.getHour();
  tm.Minute = rtc_pcf.getMinute();
  tm.Second = rtc_pcf.getSecond();
}

void RTC::set(tmElements_t tm)
{
  time_t t = makeTime(tm);
  breakTime(t, tm);
  rtc_pcf.setDate(tm.Day, tm.Wday - 1, tm.Month, 0,
                  tmYearToY2k(tm.Year));
  rtc_pcf.setTime(tm.Hour, tm.Minute, tm.Second);
  clearAlarm();
}

void RTC::_PCFConfig(String datetime)
{ // YYYY:MM:DD:HH:MM:SS
  tmElements_t tm;
  tm.Year = CalendarYrToTm(_getValue(datetime, ':', 0).toInt());
  tm.Month  = _getValue(datetime, ':', 1).toInt();
  tm.Day    = _getValue(datetime, ':', 2).toInt();
  tm.Hour   = _getValue(datetime, ':', 3).toInt();
  tm.Minute = _getValue(datetime, ':', 4).toInt();
  tm.Second = _getValue(datetime, ':', 5).toInt();
  time_t t  = makeTime(tm);
  breakTime(t, tm);
  rtc_pcf.setDate(tm.Day, tm.Wday - 1, tm.Month, 0,
                  tmYearToY2k(tm.Year));
  rtc_pcf.setTime(tm.Hour, tm.Minute, tm.Second);
}

String RTC::_getValue(String data, char separator, int index)
{
  int found      = 0;
  int strIndex[] = {0, -1};
  int maxIndex   = data.length() - 1;
  for (int i = 0; i <= maxIndex && found <= index; i++)
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
