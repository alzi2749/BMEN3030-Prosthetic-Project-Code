#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "19b10000-e8f2-537e-4f6c-d104768a1214"
#define CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define EMG_PIN A0
#define READ_INTERVAL_MS 20

BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) override {
    deviceConnected = true;
    Serial.println("Feather connected");
  }
  void onDisconnect(BLEServer* pServer) override {
    deviceConnected = false;
    Serial.println("Feather disconnected - restarting advertising...");
    pServer->startAdvertising();
  }
};

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  BLEDevice::init("MYOWARE1");
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService* pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
  );
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  BLEDevice::getAdvertising()->addServiceUUID(SERVICE_UUID);
  BLEDevice::startAdvertising();

  Serial.println("Advertising as MYOWARE1...");
}

void loop() {
  static unsigned long lastRead = 0;

  if (millis() - lastRead >= READ_INTERVAL_MS) {
    lastRead = millis();

    //int emgValue = analogRead(EMG_PIN); Where we would read
    String emgString = "";
    for( int i = 0; i < 4; i ++){
    int emgValue = random(-135, 135);
    emgString += String(emgValue) + ",";
    } 
    int emgValue = random(-135, 135);
    emgString += String(emgValue);

    delay(2000);
    emgString = "135,135,135,135,135";
    pCharacteristic->setValue(emgString.c_str());
    pCharacteristic->notify();
    delay(2000);
    emgString = "-135,0,0,0,0";
    pCharacteristic->setValue(emgString.c_str());
    pCharacteristic->notify();


    if (deviceConnected) {
      pCharacteristic->setValue(emgString.c_str());
      pCharacteristic->notify();
    }
  }
}