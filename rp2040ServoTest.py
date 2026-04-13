import time
from servo import Calibration, Servo, servo2040, ANGULAR

WIDE_ANGLE_RANGE = 270

# 1. Create a range of all servo pins from SERVO_1 to SERVO_18
servo_pins = range(servo2040.SERVO_1, servo2040.SERVO_18 + 1)
servos = []

# 2. Initialize and calibrate all servos
for pin in servo_pins:
    s = Servo(pin, ANGULAR)
    
    # Apply the wide angle (270 degree) calibration
    cal = s.calibration()
    cal.first_value(-WIDE_ANGLE_RANGE / 2) # Maps lower limit to -135
    cal.last_value(WIDE_ANGLE_RANGE / 2)   # Maps upper limit to +135
    s.calibration(cal)
    
    servos.append(s)

# 3. Enable all servos and send to min angle
print("Enabling all servos and moving to minimum angle (-135 degrees)...")
for s in servos:
    s.enable()
    s.value(-135) # Automatically sends the servo to your calibrated first_value
    
# 4. Hold position until the user stops the script
print("Holding at minimum angle. Press Stop (Ctrl+C) to exit and disable.")
try:
    while True:
        # Keep the script alive so the servos hold their position
        time.sleep(1)

except KeyboardInterrupt:
    # Safely disable all servos when you stop the script
    print("\nDisabling all servos...")
    for s in servos:
        s.disable()
    print("Test stopped.")