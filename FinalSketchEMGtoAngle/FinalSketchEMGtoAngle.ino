#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID        "19b10000-e8f2-537e-4f6c-d104768a1214"
#define CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define EMG_PIN 39
#define READ_INTERVAL_MS 20

BLECharacteristic* pCharacteristic = nullptr;
bool deviceConnected = false;

// --- Moving Average Variables ---
const int numReadings = 5;
int readings[numReadings] = {0, 0, 0, 0, 0 };
int readIndex = 0;
int total = 0;
int averageEMG = 0;

// --- Servo Control Variables ---
float currentAngle = -135.0;      
const float MAX_ANGLE = 135.0; 
const float MIN_ANGLE = -135.0;

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
  
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  // ESP32 ADC resolution is 12-bit (0-4095)
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

    // 1. Calculate the 3-point moving average
    total = total - readings[readIndex];       
    readings[readIndex] = analogRead(EMG_PIN); 
    total = total + readings[readIndex];       
    readIndex = readIndex + 1;                 

    if (readIndex >= numReadings) {
      readIndex = 0;                           
    }
    averageEMG = total / numReadings;          

    // 2. Threshold Check & Direct Speed Calculation
    // New threshold set to 250
    if (averageEMG > 250) {
      // DIRECT CORRELATION: Smaller EMG = Slower speed | Higher EMG = Higher Speed
      // Map the new threshold 250-4095 to 50-500. 
      // Divided by 100.0, this gives a step size from 0.5 degrees (slow) up to 5.0 degrees (fast) per 20ms.
      float stepIncrement = map(averageEMG, 250, 4095, 100, 750) / 100.0;
      
      currentAngle += stepIncrement; 

      // Cap the maximum angle at 135 (Fully Closed)
      if (currentAngle > 135.0) {
        currentAngle = 135.0;
      }
    } else {
      // Relax the hand back to -135 (Fully Open) when the muscle is resting
      // You can adjust this '2.0' value if you want the hand to open faster or slower
      currentAngle -= 5.0; 
      if (currentAngle < -135.0) {
        currentAngle = -135.0;
      }
    }


 // 3. Format into a String (e.g., "0,0,0,0,0\n")
    int angleInt = (int)currentAngle + 135;
    String emgString = String(angleInt) + "," + 
                       String(angleInt) + "," + 
                       String(angleInt) + "," + 
                       String(angleInt) + "," + 
                       String(angleInt) + "\n";

    // --- NEW: Print to Serial Monitor so you can see it working! ---
    Serial.print("Raw EMG: ");
    Serial.print(averageEMG);
    Serial.print(" | Sending: ");
    Serial.print(emgString);

    // 4. Send over BLE
    if (deviceConnected) {
      pCharacteristic->setValue(emgString.c_str());
      pCharacteristic->notify();
    }
  }
}