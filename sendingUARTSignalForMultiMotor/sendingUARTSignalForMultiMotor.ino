#include <Arduino.h>

// Use Serial2 for ESP32 UART
// Connect ESP32 TX2 (33) to Servo 2040 RX (GP21)
// Connect ESP32 RX2 (27) to Servo 2040 TX (GP20)
#define RXD2 27
#define TXD2 33

void setup() {
  // Start the USB Serial communication with your computer
  Serial.begin(115200);
  
  // Start the Hardware Serial communication with the Servo 2040
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  
  Serial.println("ESP32 Manual Servo Controller Started.");
  Serial.println("Type 5 comma-separated angles (e.g., 45.0,-10.0,0.0,135.0,-135.0) and hit Enter.");
}

void loop() {
  // Check if there is data coming from the computer's Serial Monitor
  if (Serial.available() > 0) {
    
    // Read the input until you press Enter (newline)
    String input = Serial.readStringUntil('\n');
    
    // Clean up any accidental invisible characters (like carriage returns)
    input.trim();
    
    // Make sure you didn't just hit Enter on an empty line
    if (input.length() > 0) {
      
      // Forward the exact string to the Servo 2040, adding a newline at the end
      Serial2.println(input);
      
      // Print a confirmation back to your screen so you know it sent
      Serial.println("Sent to servos: " + input);
    }
  }
}