import time
import math
from servo import Servo, servo2040

# 1. Initialize the servo
# Plug your servo into the header marked "1" on the board.
# If you are using a different slot, change SERVO_1 to SERVO_2, SERVO_3, etc.
test_servo = Servo(servo2040.SERVO_1)

# Enable the servo (usually enabled by default when initialized, but good practice)
test_servo.enable()

print("Starting Servo 2040 Test...")

try:
    # --- Part 1: Stepped Movements ---
    print("Moving to Minimum...")
    test_servo.to_min()
    time.sleep(1.5)

    print("Moving to Center...")
    test_servo.to_mid()
    time.sleep(1.5)

    print("Moving to Maximum...")
    test_servo.to_max()
    time.sleep(1.5)
    
    print("Moving back to Center...")
    test_servo.to_mid()
    time.sleep(1.5)

    # --- Part 2: Smooth Sweeping ---
    print("Starting smooth sweep. Press Ctrl+C in the console to stop.")
    
    # The .value() method takes a float between -1.0 (min) and 1.0 (max)
    # We use a sine wave to create a smooth sweeping motion
    while True:
        # Generate a time-based value for our sine wave
        t = time.ticks_ms() / 1000.0
        
        # Calculate a value between -1.0 and 1.0
        sweep_value = math.sin(t * 2) 
        
        # Apply the value to the servo
        test_servo.value(sweep_value)
        
        # Small delay to keep the loop from running too fast
        time.sleep(0.02)

except KeyboardInterrupt:
    # Gracefully handle stopping the script
    print("Test stopped by user.")
    
    # Disable the servo to stop it from drawing power and holding its position
    test_servo.disable()
    print("Servo disabled.")