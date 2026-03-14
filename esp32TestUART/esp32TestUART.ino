
#define RXD2 27
#define TXD2 33

void setup() {
  // 1. Start standard Serial computer
  Serial.begin(115200);
  
  // 2. Start Serial2 for the Servo2040
  // SERIAL_8N1 is the standard communication protocol
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  delay(1000);
  Serial.println("Dual UART setup complete. Starting test...");
}

void loop() {
  // Test 1: Move to -90 degrees
  Serial.println("Computer Monitor: Commanding -90.0 degrees"); 
  Serial2.println("-90.0"); // Actually sends the data to the Servo 2040
  delay(2000);

  // Test 2: Move to 0 degrees (Center)
  Serial.println("Computer Monitor: Commanding 0.0 degrees");
  Serial2.println("0.0");
  delay(2000);

  // Test 3: Move to 90 degrees
  Serial.println("Computer Monitor: Commanding 90.0 degrees");
  Serial2.println("90.0");
  delay(2000);
}