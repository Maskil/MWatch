#ifndef _BLE_H_
#define _BLE_H_

#include "Arduino.h"

#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string.h>
#include "esp_ota_ops.h"
#include "config.h"

class BLE;

class BLE {
public:
  BLE(void);
  ~BLE(void);

  bool begin(const char *serv_uuid, const char *chara_uuid);
  bool updateStatus();
  char* getData();

private:
  String local_name;

  BLEServer *pServer                  = NULL;
  BLEService *pService                = NULL;
  BLEAdvertising *pAdvertising        = NULL;
  BLECharacteristic *pCharacteristic  = NULL;

};

#endif
