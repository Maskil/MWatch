#include "BLE.h"

bool connected;
char data[64];

class MWSCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { connected = true; }
  void onDisconnect(BLEServer *pServer) { connected = false; }
};

class MWCCallback : public BLECharacteristicCallbacks {
public:
  void onWrite(BLECharacteristic *pCharacteristic);
};

void MWCCallback::onWrite(BLECharacteristic *pCharacteristic) {
  const char* value = (pCharacteristic->getValue()).c_str();
  memset(data,0,64*sizeof(char));
  if(!value[0]) return;
  memcpy(data, value, strlen(value)*sizeof(value));
}

// Constructor
BLE::BLE(void) {}
// Destructor
BLE::~BLE(void) {}

bool BLE::begin(const char *serv_uuid, const char *chara_uuid) {
  memset(data, 0, 64*sizeof(char));
  BLEDevice::init("MWatch");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MWSCallbacks());

  // service
  pService = pServer->createService(serv_uuid);
  // characteristic
  pCharacteristic = pService->createCharacteristic(chara_uuid,
                                                   BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MWCCallback());

  pCharacteristic->setValue("hello world");

  // Start the service(s)
  pService->start();

  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(serv_uuid);
  pAdvertising->setScanResponse(true);
  BLEDevice::startAdvertising();

  return true;
}

char* BLE::getData() {return data;}

bool BLE::updateStatus() { return connected; }

