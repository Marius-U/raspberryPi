import RPi.GPIO as GPIO
import time

# Define GPIO pins for motor control
MOTOR_IN1 = 22  # Change this to the desired GPIO pin
MOTOR_IN2 = 23  # Change this to the desired GPIO pin
MOTOR_IN3 = 24  # Change this to the desired GPIO pin
MOTOR_IN4 = 25  # Change this to the desired GPIO pin

# Initialize GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(MOTOR_IN1, GPIO.OUT)
GPIO.setup(MOTOR_IN2, GPIO.OUT)
GPIO.setup(MOTOR_IN3, GPIO.OUT)
GPIO.setup(MOTOR_IN4, GPIO.OUT)

# Set both PWM pins (GPIO 12 and 13) to HIGH
GPIO.setup(12, GPIO.OUT)
GPIO.setup(13, GPIO.OUT)
GPIO.output(12, GPIO.HIGH)
GPIO.output(13, GPIO.HIGH)

# Example: Run motor 1 forward
GPIO.output(MOTOR_IN1, GPIO.HIGH)
GPIO.output(MOTOR_IN2, GPIO.LOW)

# Example: Run motor 2 backward
GPIO.output(MOTOR_IN3, GPIO.LOW)
GPIO.output(MOTOR_IN4, GPIO.HIGH)

# Run the motors for some time (e.g., 5 seconds)
time.sleep(5)

# Stop the motors
GPIO.output(MOTOR_IN1, GPIO.LOW)
GPIO.output(MOTOR_IN2, GPIO.LOW)
GPIO.output(MOTOR_IN3, GPIO.LOW)
GPIO.output(MOTOR_IN4, GPIO.LOW)

# Cleanup GPIO
GPIO.cleanup()
