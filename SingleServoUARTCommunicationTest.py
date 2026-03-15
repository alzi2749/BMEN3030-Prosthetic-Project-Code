import time
from machine import UART, Pin
from servo import Servo, servo2040

# 1. Initialize the Servo
my_servo = Servo(servo2040.SERVO_1)
my_servo.enable()
my_servo.to_mid() # Start at the center position

# 2. Set up UART on SDA (GP20) and SCL (GP21)
# RP2040 Hardware UART1 gp20 and gp21
uart = UART(1, baudrate=115200, tx=Pin(20), rx=Pin(21))

print("Listening for ESP32 UART signals on GP20 and GP21...")

while True:
    # Check if there is data waiting in the UART buffer
    if uart.any() > 0:
        # Read a line of data (expects ESP32 to append a newline '\n')
        data = uart.readline()
        
        if data:
            try:
                # Decode bytes to a string and strip whitespace/newlines
                command = data.decode('utf-8').strip()
                
                # Convert the command to a float (expecting an angle from -90 to 90)
                angle = float(command)
                
                # Clamp the angle to ensure it doesn't exceed physical limits
                angle = max(-90.0, min(90.0, angle))
                
                # Command the servo to move
                my_servo.value(angle)
                print(f"Success: Moved to {angle} degrees (Value: {servo_val})")
                
            except ValueError:
                # This gracefully handles garbled data or non-numeric text
                print(f"Error: Ignored invalid data -> {data}")
                
    # A small delay prevents this loop from completely hogging the CPU
    time.sleep(0.01)