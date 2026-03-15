import time
from machine import UART, Pin
from servo import Calibration, Servo, servo2040, ANGULAR, LINEAR, CONTINUOUS

# 1. Initialize 5 Servos 
# Note: Spacing them out (e.g., 1, 5, 9, 13, 17) can help with cable clutter,
# but using 1-5 is electrically fine for standard servos.
pins = [servo2040.SERVO_1, servo2040.SERVO_2, servo2040.SERVO_3, servo2040.SERVO_4, servo2040.SERVO_5]
servos = []
WIDE_ANGLE_RANGE = 270



# 2. Loop through each pin to initialize and calibrate
for pin in pins:
    # Create the servo object
    s = Servo(pin, ANGULAR)
    
    # Access and modify its calibration
    cal = s.calibration()
    cal.first_value(-WIDE_ANGLE_RANGE / 2) # Maps lower limit to -135
    cal.last_value(WIDE_ANGLE_RANGE / 2)   # Maps upper limit to +135

    
    # Apply the modified calibration
    s.calibration(cal)
    
    # Enable and send to the neutral resting position (0 degrees / 1500us)
    s.enable()
    s.to_mid()
    
    # Store the calibrated servo in our list
    servos.append(s)

print(f"Successfully calibrated {len(servos)} servos to a {WIDE_ANGLE_RANGE} degree range.")


# 3. Set up UART on SDA (GP20) and SCL (GP21)
# RP2040 Hardware UART1 gp20 and gp21
uart = UART(1, baudrate=115200, tx=Pin(20), rx=Pin(21))

print("Listening for 5-motor CSV data on GP20/GP21...")

while True:
    if uart.any() > 0:
        data = uart.readline()
        if data:
            try:
                # Decode and split the string by commas
                decoded = data.decode('utf-8').strip()
                parts = decoded.split(',')
                
                # Ensure we have enough values for all 5 motors
                if len(parts) == 5:
                    for i in range(5):
                        angle = float(parts[i])
                        # Clamp and move
                        angle = max(-135.0, min(135.0, angle))
                        servos[i].value(angle)
                    
                    print(f"Moved 5 motors to: {parts}")
                else:
                    print(f"Error: Expected 5 values, got {len(parts)}")
                    
            except Exception as e:
                print(f"Error: {e}")
                
    time.sleep(0.01)
