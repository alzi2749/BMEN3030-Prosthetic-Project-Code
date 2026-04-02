import gc
import time
from machine import Pin
from pimoroni import Analog, AnalogMux, Button
from plasma import WS2812
from servo import Servo, servo2040  # Changed from ServoCluster to Servo

"""
Servo 2040 Motor 1 Current Test (Single Servo)
Sweeps Motor 1 back and forth, measuring its current draw
at every step and displaying it on the console and LED bar.

Press the "Boot" button to exit the program.
"""

BRIGHTNESS = 0.4      # The brightness of the LEDs
MAX_CURRENT = 2.0     # 2.0 Amps max for the LED visual scale
SAMPLES = 5           # Samples per reading
TIME_BETWEEN = 0.001  # Time between ADC readings

# Free up hardware resources
gc.collect()

# --- THE SIMPLIFIED SERVO SETUP ---
# Initialize just Servo 1 on its own
motor1 = Servo(servo2040.SERVO_1)

# Set up current sensing (ADC and Multiplexer)
cur_adc = Analog(servo2040.SHARED_ADC, servo2040.CURRENT_GAIN,
                 servo2040.SHUNT_RESISTOR, servo2040.CURRENT_OFFSET)
mux = AnalogMux(servo2040.ADC_ADDR_0, servo2040.ADC_ADDR_1, servo2040.ADC_ADDR_2,
                muxed_pin=Pin(servo2040.SHARED_ADC))

# Set up the LED bar and Button
led_bar = WS2812(servo2040.NUM_LEDS, 1, 0, servo2040.LED_DATA)
user_sw = Button(servo2040.USER_SW)

led_bar.start()

# Enable the single servo
motor1.enable()




print("Testing Motor 1 Current Draw (Single Servo Mode).")
print("Press 'BOOT' to stop.")

# Helper function to read current and update the LEDs
def measure_and_display(position):
    mux.select(servo2040.CURRENT_SENSE_ADDR)
    
    current = 0
    for _ in range(SAMPLES):
        current += cur_adc.read_current()
        time.sleep(TIME_BETWEEN)
    current /= SAMPLES
    
    # Print the live data to the console
    print(f"Position: {position:5.2f} | Current: {current:5.3f} A")
    
    # Update the LED Bar
    percent = (current / MAX_CURRENT)
    for i in range(servo2040.NUM_LEDS):
        hue = (1.0 - i / (servo2040.NUM_LEDS - 1)) * 0.333
        level = (i + 0.5) / servo2040.NUM_LEDS
        if percent >= level:
            led_bar.set_hsv(i, hue, 1.0, BRIGHTNESS)
        else:
            led_bar.set_hsv(i, hue, 1.0, 0.0)

# --- SWEEP LOGIC ---
position = 0.0
step = 5
direction = 1

# Run until the BOOT button is pressed
while not user_sw.raw():
    
    motor1.value(position)
    
    # 2. Measure current and update lights
    measure_and_display(position)
    
    # 3. Calculate the next position
    position += step * direction
    
    # 4. Reverse direction if we hit the limits (-1.0 or 1.0)
    if position >= 90.0:
        position = 90.0
        direction = -1
    elif position <= -90.0:
        position = -90.0
        direction = 1
        
    # Small delay so the sweep isn't violently fast
    time.sleep(0.02) 

# Clean up safely when stopped
motor1.disable()
led_bar.clear()
print("\nTest finished.")