#include "mwatch.h"

RTC MWatch::rtc;
GxEPD2_BW<Display, Display::HEIGHT> MWatch::display(
  Display(DISPLAY_CS, DISPLAY_DC, DISPLAY_RES, DISPLAY_BUSY));

RTC_DATA_ATTR uint8_t guiState = 0;
RTC_DATA_ATTR uint8_t menuIndex;
RTC_DATA_ATTR uint8_t wifiIndex;
RTC_DATA_ATTR uint8_t y_pos = 0;
RTC_DATA_ATTR bool displayFullInit = true;
RTC_DATA_ATTR bool alreadyInMenu   = false;
RTC_DATA_ATTR uint8_t iclock = 0;
RTC_DATA_ATTR char lastConnectedSSID[64];
RTC_DATA_ATTR float batteryVoltage = 0;
RTC_DATA_ATTR uint16_t gmtOffset = 0;
RTC_DATA_ATTR long lastTimeout;
RTC_DATA_ATTR weatherData currentWeather;
RTC_DATA_ATTR SMTPSession smtp;
RTC_DATA_ATTR tmElements_t currentTime;
RTC_DATA_ATTR const char *menuItems[] = {"About", "Sync", "Mail", "Bluetooth", "Research"};

void MWatch::init()
{
  Wire.begin(SDA, SCL);
  rtc.init();
  display.epd2.selectSPI(SPI, SPISettings(20000000, MSBFIRST, SPI_MODE0));
  display.init(0, displayFullInit, 10, true);
  display.epd2.setBusyCallback(displayBusyCallback);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&Inconsolata_small);
  switch (esp_sleep_get_wakeup_cause()) {
  case ESP_SLEEP_WAKEUP_EXT0: // rtc Alarm
    rtc.read(currentTime);
    iclock++;
    switch (guiState){
    case WATCHFACE_STATE:
      showWatchFace();
      break;
    case MAIN_MENU_STATE:
      alreadyInMenu = !alreadyInMenu;
      stateChange(alreadyInMenu, 0);
      break;
    }
    break;
  case ESP_SLEEP_WAKEUP_EXT1: // button Press
    handleButtonPress();
    break;
  default: // reset
    rtc.config();
    rtc.read(currentTime);
    showWatchFace();
    vibMotor(127, 1);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    displayFullInit = false;
    break;
  }
  deepSleep();
}

void MWatch::displayBusyCallback(const void *)
{
  gpio_wakeup_enable((gpio_num_t)DISPLAY_BUSY, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
  esp_light_sleep_start();
}

void MWatch::deepSleep()
{
  display.hibernate();
  rtc.clearAlarm();
  uint64_t ignore = 0b11110001000000110000100111000010;
  for (uint8_t i = 0; i < GPIO_NUM_MAX; i++, ignore >>= 1)
    if (!(ignore & 0b1))
      pinMode(i, INPUT);
  esp_sleep_enable_ext0_wakeup((gpio_num_t)RTC_INT_PIN, 0); // wake on rtc interrupt
  esp_sleep_enable_ext1_wakeup(BTN_PIN_MASK, ESP_EXT1_WAKEUP_ANY_HIGH); // wake on button press
  esp_deep_sleep_start();
}

void MWatch::handleButtonPress()
{
  lastTimeout = millis();
  pinMode(MENU_BTN_PIN, INPUT); pinMode(UP_BTN_PIN, INPUT);
  pinMode(BACK_BTN_PIN, INPUT); pinMode(DOWN_BTN_PIN, INPUT);
  if (millis() - lastTimeout > 5000) return;
  if (digitalRead(MENU_BTN_PIN))
    stateChange(min(guiState+1, APP_STATE), 0);
  else if (digitalRead(BACK_BTN_PIN))
    stateChange(max(guiState-3, WATCHFACE_STATE), 0);
  else if (digitalRead(UP_BTN_PIN))
    stateChange(MAIN_MENU_STATE, guiState);
  else if (digitalRead(DOWN_BTN_PIN))
    stateChange(MAIN_MENU_STATE, -guiState);
}

void MWatch::stateChange(uint8_t state, int8_t direction)
{
  guiState = state;
  switch (guiState){
  case WATCHFACE_STATE:
    showWatchFace();
    break;
  case MAIN_MENU_STATE:
    display.fillScreen(GxEPD_WHITE);
    if (direction < 0)
      menuIndex = (menuIndex + 1) % MENU_LENGTH;
    else if (direction > 0)
      menuIndex = menuIndex - 1 - min(menuIndex - 1, 0) * MENU_LENGTH;
    showMenu();
    break;
  case APP_STATE:
    display.fillScreen(GxEPD_WHITE);
    switch (menuIndex){
    case 0: showAbout(); break;
    case 1: showSyncNTP(); break;
    case 2: showMail(1); break;
    case 3: showBLE(); break;
    case 4: showMast(); break;
    default: break;
    }
  }
}

void MWatch::showMail(uint8_t attempt)
{
  display.setCursor(0, MENU_HEIGHT);
  if (attempt > 3) {
    exit("Failed 3 times\nAbort");
    return;
  }
  display.println("Mail; Attempt # " + String(attempt) + "\n" + settings.sender_name + "\nto\n" + settings.recipient_name);
  display.display(true);
  Session_Config config;
  SMTP_Message message;
  config.server.host_name = settings.host_name;
  config.server.port = settings.port;
  config.login.email = settings.login_email;
  config.login.password = settings.app_pass;
  config.time.gmt_offset = gmtOffset/3600;
  config.time.ntp_server = settings.ntpServer;
  message.sender.name = settings.sender_name;
  message.sender.email = settings.sender_email;
  message.subject = settings.subject;
  message.addRecipient(settings.recipient_name, settings.recipient_email);
  message.text.content = settings.body;
  if (!connectWiFi()) {
    exit("Internet failed");
    return;
  } else {
    display.println("Internet OK");
    display.display(true);
  }
  if (!smtp.connect(&config) || !MailClient.sendMail(&smtp, &message)) {
    display.fillScreen(GxEPD_WHITE);
    showMail(attempt + 1);
    return;
  }
  exit("Sent OK");
  WiFi.mode(WIFI_OFF);
}

void MWatch::showWatchFace()
{
  if (iclock == 60) {
    if (connectWiFi()) fetchWeatherData();
    getBatteryVoltage();
    iclock = 0;
  }
  display.drawBitmap(0, 0, image, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);
  display.setCursor(0, MENU_HEIGHT);
  display.println((currentTime.Hour<10?"0":"") + String(currentTime.Hour) + ":" +
                  (currentTime.Minute<10?"0":"") + String(currentTime.Minute) + " " +
                  (currentTime.Month<10?"0":"")+String(currentTime.Month)+"/"+
                  (currentTime.Day<10?"0":"")+String(currentTime.Day));
  display.println("CUR:"+String(currentWeather.temperature)+"("+String(currentWeather.feels_like)+") " +
                  String(currentWeather.min)+" ~ "+String(currentWeather.max));
  display.print("V=" + String(batteryVoltage) + " " + String(max(int(batteryVoltage*222)-833, 0)) + "%");
  display.display(!displayFullInit);
}

void MWatch::showMenu()
{
  y_pos = MENU_HEIGHT;
  for (uint8_t i = 0; i < MENU_LENGTH; ++i, y_pos += MENU_HEIGHT) {
    display.setCursor(5, y_pos);
    display.print(menuItems[i]);
  }
  display.setCursor(180, MENU_HEIGHT*(menuIndex+1));
  display.print("*");
  display.display(true);
}

void MWatch::showAbout()
{
  getBatteryVoltage();
  display.setCursor(0, MENU_HEIGHT);
  display.println("MWATCH V1.0\nU: " + settings.sender_name);
  display.println("RTC: "+String(rtc.rtc_success?("PCF8563"):("UNKNOWN")));
  display.println("BAT: "+String(batteryVoltage)+" V");
  display.display(true);
}

void MWatch::showMast()
{
  display.setCursor(0, MENU_HEIGHT);
  display.println("Research Mode");
  display.display(true);
  vibMotor(1000, 20);
  exit("");
}

void MWatch::vibMotor(uint32_t intervalMs, uint32_t length)
{
  pinMode(VIB_MOTOR_PIN, OUTPUT);
  bool motorOn = false;
  for (uint16_t i = 0; i < length; ++i) {
    motorOn = !motorOn;
    digitalWrite(VIB_MOTOR_PIN, motorOn);
    delay(intervalMs);
  }
}

void MWatch::getBatteryVoltage()
{
  batteryVoltage = analogReadMilliVolts(BATT_ADC_PIN)/500.0f;
}

void MWatch::fetchWeatherData()
{
  String query = "https://api.openweathermap.org/data/2.5/weather?id=" + settings.cityID +
    String("&units=metric&lang=en&appid=") + settings.weatherAPIKey;
  HTTPClient http;
  http.setConnectTimeout(3000); // 3s timeout
  http.begin(query.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode == 200) {
    strcpy(lastConnectedSSID, WiFi.SSID().c_str());
    JSONVar responseObject     = JSON.parse(http.getString());
    currentWeather.temperature = int(responseObject["main"]["temp"]);
    currentWeather.feels_like  = int(responseObject["main"]["feels_like"]);
    currentWeather.min = int(responseObject["main"]["temp_min"]);
    currentWeather.max = int(responseObject["main"]["temp_max"]);
    gmtOffset = int(responseObject["timezone"]);
    syncNTP();
  }
  http.end();
}

bool MWatch::connectWiFi(String SSID, String pass)
{
  if (WiFi.begin(SSID, pass) != WL_CONNECT_FAILED && WL_CONNECTED == WiFi.waitForConnectResult())
    return true;
  WiFi.mode(WIFI_OFF);
  return false;
}

bool MWatch::connectWiFi()
{
  int8_t num = WiFi.scanNetworks();
  for(uint8_t i=0; i<num; ++i)
    for(uint8_t j=0; j<settings.wifi_len; ++j)
      if (strcmp(WiFi.SSID(i).c_str(), settings.wifi_list[j][0])==0 &&
          WiFi.begin(settings.wifi_list[j][0], settings.wifi_list[j][1])!=WL_CONNECT_FAILED &&
          WL_CONNECTED == WiFi.waitForConnectResult())
        return true;
  WiFi.mode(WIFI_OFF);
  return false;
}

void MWatch::showBLE()
{
  display.setCursor(0, MENU_HEIGHT);
  display.println("BLE");
  BLE BT;
  if (!BT.begin(settings.serv_uuid.c_str(), settings.chara_uuid.c_str())) {
    exit("Init failed");
    return;
  }
  display.print("Init OK\nMESSAGE: ");
  display.display(true);
  char *value = BT.getData();
  for (uint8_t i = 0; i < 100 && !value[0]; ++i){
    delay(500);
    value = BT.getData();
  }
  btStop();
  exit((value[0])?value:"Timeout\nAbort");
}

void MWatch::showSyncNTP()
{
  display.setCursor(0, MENU_HEIGHT);
  display.println("Sync time & weather\nGMT offset: "+String(gmtOffset/3600));
  display.display(true);
  if (!connectWiFi()) {exit("WiFi failed"); return;}
  if (!syncNTP()) {exit("NTP Sync failed"); return;}
  fetchWeatherData();
  rtc.read(currentTime);
  WiFi.mode(WIFI_OFF);
  exit("Sync Success To\n"+String(lastConnectedSSID)+"\nCurrent Time:\n"+
       String(currentTime.Month)+"/"+String(currentTime.Day)+" "+
       String(currentTime.Hour)+":"+String(currentTime.Minute));
}

void MWatch::exit(String message)
{
  display.println((message == "") ? "Exiting..." : message);
  display.display(true);
  delay(3000);
  stateChange(MAIN_MENU_STATE, 0);
}

bool MWatch::syncNTP()
{
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, settings.ntpServer.c_str(), gmtOffset);
  timeClient.begin();
  if (!timeClient.forceUpdate())
    return false;
  breakTime((time_t)timeClient.getEpochTime(), currentTime);
  rtc.set(currentTime);
  return true;
}
