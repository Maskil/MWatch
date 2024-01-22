#ifndef MWATCH_H
#define MWATCH_H

#include <Arduino.h>
#include <ESP_Mail_Client.h>
#include <HTTPClient.h>
#include <NTPClient.h>
#include <Arduino_JSON.h>
#include <GxEPD2_BW.h>
#include <Wire.h>
#include "Inconsolata_10pt.h"
#include "Image.h"
#include "Display.h"
#include "RTC.h"
#include "BLE.h"
#include "config.h"

typedef struct weatherData {
  int8_t temperature;
	int8_t feels_like;
  int8_t min;
  int8_t max;
  char weatherDescription[32];
  bool external;
} weatherData;

typedef struct Settings {
  // Weather Settings
  String cityID;
  String weatherAPIKey;
  String weatherURL;
  // NTP Settings
  String ntpServer;
  char wifi_list[10][2][32];
  uint8_t wifi_len;
  // Mail Settings
  String host_name;
  uint16_t port;
  String login_email;
  String sender_email;
  String sender_name;
  String recipient_email;
  String recipient_name;
  String app_pass;
  String subject;
  String body;
} Settings;

class MWatch {
public:
  static RTC rtc;
  static GxEPD2_BW<Display, Display::HEIGHT> display;
  Settings settings;

public:
  explicit MWatch(const Settings &s) : settings(s) {} // constructor
  void init();
  void deepSleep();
  static void displayBusyCallback(const void *);
  void getBatteryVoltage();
  void vibMotor(uint32_t intervalMs = 100, uint32_t length = 20);
  void stateChange(uint8_t state, int8_t direction);
  void handleButtonPress();
  void showMail(uint8_t attempt);
  void showMenu();
  void showAbout();
  void showMast();
  void showBLE();
  void showSyncNTP();
  bool syncNTP();
  void setTime();
  bool connectWiFi();
  bool connectWiFi(String SSID, String pass);
  void fetchWeatherData();
  void showWatchFace();
  void drawPartialFace();
  void exit(String);
};

#endif
