#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEClient.h>

#define SERVICE_UUID        "19b10000-e8f2-537e-4f6c-d104768a1214"
#define CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define RXD2 27
#define TXD2 33


BLEClient* pClient = nullptr;
BLERemoteCharacteristic* pRemoteCharacteristic = nullptr;
BLEAdvertisedDevice* myowareDevice = nullptr;
bool doConnect = false;
bool connected = false;

// New variables to handle incoming BLE data safely
String latestBLEData = "";
volatile bool newBLEDataAvailable = false;

void notifyCallback(BLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
  String received = "";
  for (int i = 0; i < length; i++) {
    received += (char)pData[i];
  }
  
  // Save the data to the global variable and set the flag
  latestBLEData = received;
  newBLEDataAvailable = true;
  
  Serial.print("BLE Received: ");
  Serial.println(received);
}

class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == "MYOWARE1") {
      Serial.println("Found MYOWARE1, stopping scan...");
      BLEDevice::getScan()->stop();
      myowareDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

bool connectToMyoWare() {
  pClient = BLEDevice::createClient();

  if (!pClient->connect(myowareDevice)) {
    Serial.println("Connection failed");
    return false;
  }
  Serial.println("Connected to MYOWARE1");

  BLERemoteService* pService = pClient->getService(SERVICE_UUID);
  if (!pService) {
    Serial.println("Service not found");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic = pService->getCharacteristic(CHARACTERISTIC_UUID);
  if (!pRemoteCharacteristic) {
    Serial.println("Characteristic not found");
    pClient->disconnect();
    return false;
  }

  pRemoteCharacteristic->registerForNotify(notifyCallback);
  connected = true;
  Serial.println("Subscribed - receiving EMG strings...");
  return true;
}

void setup() {
  Serial.begin(115200);
  BLEDevice::init("ESP32 Feather");
  // Start the USB Serial communication with your computer
  
  // Start the Hardware Serial communication with the Servo 2040
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  Serial.println("ESP32 Manual Servo Controller Started.");
  Serial.println("Type 5 comma-separated angles (e.g., 45.0,-10.0,0.0,135.0,-135.0) and hit Enter.");

  BLEScan* pScan = BLEDevice::getScan();
  pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
  pScan->setActiveScan(true);
  pScan->start(10);

  Serial.println("Scanning for MYOWARE1...");
}

void loop() {
  if (doConnect) {
    connectToMyoWare();
    doConnect = false;
  }

  if (!connected || !pClient->isConnected()) {
    Serial.println("Disconnected - rescanning...");
    connected = false;
    delay(1000);
    BLEDevice::getScan()->start(10);
  }

  // --- NEW: PROCESS INCOMING BLE DATA ---
  if (newBLEDataAvailable) {
    // 1. Immediately turn the flag off so we don't process the same data twice
    newBLEDataAvailable = false; 
    
    // 2. Do whatever you want with latestBLEData! 
    // For example, forwarding the MYOWARE string straight to the Servo 2040:
    Serial2.println(latestBLEData);
    Serial.println("Forwarded BLE data to servos: " + latestBLEData);
  }

  // --- FIXED: PROCESS INCOMING SERIAL COMMANDS (From Computer) ---
  if (Serial.available() > 0) {
    // Read the incoming text from the Serial monitor
    String input = Serial.readStringUntil('\n'); 
    
    // Clean up any accidental invisible characters (like carriage returns)
    input.trim();
    
    // Make sure you didn't just hit Enter on an empty line
    if (input.length() > 0) {
      // Forward the exact string to the Servo 2040, adding a newline at the end
      Serial2.println(input);
      
      // Print a confirmation back to your screen so you know it sent
      Serial.println("Sent to servos via USB: " + input);
    }
  }

  delay(10);
}