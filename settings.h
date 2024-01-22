#ifndef SETTINGS_H
#define SETTINGS_H

Settings settings {
  // weather
  .cityID = "6618620", // city ID shown on openweathermap.org. default is Paris
    .weatherAPIKey = "", // API key from openweathermap.org. API version is 2.5 (Current Weather Data)
    // time
    .ntpServer = "pool.ntp.org", // NTP server. default is pool.ntp.org
    // wifi
    .wifi_list = {
      {"wifi-A", "passwordA"},
      {"wifi-B", "passwordB"},
    },
    .wifi_len = 2, // change to actual wifi list size
    // mail
    .host_name = "smtp.gmail.com", // smtp host name. default is smtp.gmail.com
    .port = 587, // smtp port. default is 587
    .login_email = "", // email that is used to access smtp server
    .sender_email = "", // same as login_email or alias email
    .sender_name = "",
    .recipient_email = "",
    .recipient_name = "",
    .app_pass = "", // password or app password if using such
    .subject = "", // subject of the email
    .body = "", // body of the email
    // ble
    .serv_uuid = "",
    .chara_uuid = ""
    };

#endif
