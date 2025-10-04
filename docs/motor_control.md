# Motor Control Documentation

## Overview

The motor control module provides control of two DC motors via L298N H-bridge driver using:
- GPIO pins for direction control
- PWM for speed control
- FIFO-based inter-process communication

## Hardware Configuration

### L298N H-Bridge Connections

**Motor 1:**
- IN1 (GPIO 22) + IN2 (GPIO 23) = Direction
- ENA (GPIO 12 PWM) = Speed

**Motor 2:**
- IN3 (GPIO 24) + IN4 (GPIO 25) = Direction
- ENB (GPIO 13 PWM) = Speed

## Python Implementation

### Basic Motor Control

**File:** `motor/python/p_moto.py`

```python
import RPi.GPIO as GPIO

# Initialize GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(22, GPIO.OUT)  # IN1
GPIO.setup(23, GPIO.OUT)  # IN2

# Motor forward
GPIO.output(22, GPIO.HIGH)
GPIO.output(23, GPIO.LOW)

# Motor stop
GPIO.output(22, GPIO.LOW)
GPIO.output(23, GPIO.LOW)

GPIO.cleanup()
```

### FIFO-based Control

**Writer:** `motor/python/writer.py`
```python
# Write commands to FIFO
echo "fw" > /tmp/motor_control_fifo  # Forward
echo "bw" > /tmp/motor_control_fifo  # Backward
echo "st" > /tmp/motor_control_fifo  # Stop
```

## C Implementation

### Main FIFO Controller

**File:** `motor/c/moto_fifo.c`

```bash
# Compile
gcc -o motor/build/moto_fifo motor/c/moto_fifo.c -lwiringPi

# Run (listener)
./motor/build/moto_fifo
```

**Command Protocol:**
- `fw\n` - Move forward
- `bw\n` - Move backward
- `st\n` - Stop motors

**Features:**
- Non-blocking FIFO reads
- PWM speed control (adjustable)
- Automatic FIFO creation at `/tmp/motor_control_fifo`

### FIFO with Key-based Commands

**File:** `motor/c/motoFifoKey.c`

Similar to moto_fifo.c but with keyboard input processing.

### Direct Motor Control

**File:** `motor/c/moto_h.c`

Basic motor control without FIFO (for testing).

## Motor Control Logic

### Direction Control Truth Table

| IN1 | IN2 | Motor 1 Action |
|-----|-----|----------------|
| HIGH | LOW | Forward |
| LOW | HIGH | Backward |
| LOW | LOW | Stop |
| HIGH | HIGH | Brake |

### PWM Speed Control

```c
// Initialize PWM
pinMode(PWM_PIN_1, PWM_OUTPUT);
pwmSetMode(PWM_MODE_MS);
pwmSetClock(192);
pwmSetRange(2000);

// Set speed (0-2000)
pwmWrite(PWM_PIN_1, 500);   // 25% speed
pwmWrite(PWM_PIN_1, 1000);  // 50% speed
pwmWrite(PWM_PIN_1, 2000);  // 100% speed
```

## Usage Examples

### Start Motor Control Daemon

```bash
# Compile
cd motor/c
gcc -o ../build/moto_fifo moto_fifo.c -lwiringPi

# Run in background
cd ../build
./moto_fifo &
```

### Send Commands via FIFO

```bash
# Python
python motor/python/writer.py

# Shell
echo "fw" > /tmp/motor_control_fifo
sleep 2
echo "st" > /tmp/motor_control_fifo
```

### Remote Control

```python
import socket
import time

def send_motor_command(command):
    with open('/tmp/motor_control_fifo', 'w') as fifo:
        fifo.write(command + '\n')

# Move forward for 2 seconds
send_motor_command('fw')
time.sleep(2)
send_motor_command('st')
```

## Troubleshooting

### WiringPi Installation
```bash
# Install WiringPi
sudo apt-get install wiringpi

# Verify installation
gpio -v
```

### Permission Issues
```bash
# Add user to gpio group
sudo usermod -a -G gpio $USER

# Set GPIO permissions
sudo chmod 666 /dev/gpiomem
```

### FIFO Debugging
```bash
# Check if FIFO exists
ls -l /tmp/motor_control_fifo

# Monitor FIFO
tail -f /tmp/motor_control_fifo

# Remove stuck FIFO
rm /tmp/motor_control_fifo
```

### Motor Not Running
1. Check power supply (7-12V to L298N)
2. Verify GPIO connections
3. Test with LED on motor pins
4. Check enable pins (ENA/ENB) are HIGH or PWM active

## Safety Features

### Emergency Stop
```bash
# Send stop command
echo "st" > /tmp/motor_control_fifo

# Or kill the process
pkill moto_fifo
```

### Automatic Timeout
Add timeout logic to stop motors if no command received:

```c
time_t last_command = time(NULL);
if (time(NULL) - last_command > 5) {
    // Stop motors after 5 seconds
    processCommand(STOP);
}
```

## Advanced Configuration

### Speed Adjustment
Edit PWM values in `moto_fifo.c`:

```c
// Forward movement
pwmWrite(PWM_PIN_1, 500);  // Left motor
pwmWrite(PWM_PIN_2, 580);  // Right motor (adjusted for straight)
```

### Custom Commands
Extend command enum in `moto_fifo.c`:

```c
typedef enum {
    FORWARD,
    BACKWARD,
    LEFT,      // Add turning
    RIGHT,     // Add turning
    STOP,
    INVALID
} MotorCommand;
```
