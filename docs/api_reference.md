# API Reference

## Camera Module

### Python API

#### camera_stream.py (Flask + PiCamera)

```python
from flask import Flask, Response
from picamera import PiCamera

app = Flask(__name__)
camera = PiCamera()
camera.resolution = (1920, 1080)
camera.rotation = -90

@app.route('/video_feed')
def video_feed():
    """Stream camera feed via HTTP"""
    return Response(generate_frames(),
                   mimetype='multipart/x-mixed-replace; boundary=frame')
```

### C API

#### c_stream.c (V4L2)

```c
// Initialize camera
bool initCamera();

// Capture frames thread
void *captureFrames(void *arg);

// Handle client connections
void *handleClient(void *arg);

// Send frame to client
void sendFrame(unsigned char *frame);
```

### C++ API

#### stream.cpp (OpenCV + Poco)

```cpp
class StreamRequestHandler : public HTTPRequestHandler {
public:
    void handleRequest(HTTPServerRequest& req, HTTPServerResponse& resp) override;
};

// Send frames to all connected clients
void sendToAllClients(Mat frame);
```

---

## Motor Control Module

### Python API

#### p_moto.py

```python
import RPi.GPIO as GPIO

# Constants
MOTOR_IN1 = 22
MOTOR_IN2 = 23
MOTOR_IN3 = 24
MOTOR_IN4 = 25

# Initialize
GPIO.setmode(GPIO.BCM)
GPIO.setup(MOTOR_IN1, GPIO.OUT)

# Control functions
def motor_forward():
    GPIO.output(MOTOR_IN1, GPIO.HIGH)
    GPIO.output(MOTOR_IN2, GPIO.LOW)

def motor_stop():
    GPIO.output(MOTOR_IN1, GPIO.LOW)
    GPIO.output(MOTOR_IN2, GPIO.LOW)
```

#### writer.py (FIFO Writer)

```python
def write_command(command: str):
    """Write motor command to FIFO

    Args:
        command: 'fw' (forward), 'bw' (backward), 'st' (stop)
    """
    with open('/tmp/motor_control_fifo', 'w') as fifo:
        fifo.write(command + '\n')
```

### C API

#### moto_fifo.c

```c
// Motor commands enum
typedef enum {
    FORWARD,
    BACKWARD,
    STOP,
    INVALID
} MotorCommand;

// Initialize PWM
void initializePWM();

// Process motor command
void processCommand(MotorCommand command);

// Parse command string
MotorCommand getCommand(const char *command);
```

**Constants:**
```c
#define PWM_PIN_1 12
#define PWM_PIN_2 13
#define MOTOR_IN1 22
#define MOTOR_IN2 23
#define MOTOR_IN3 24
#define MOTOR_IN4 25
#define FIFO_PATH "/tmp/motor_control_fifo"
```

---

## LCD Display Module

### Python API

#### lcd_driver.py

```python
class LCD:
    def __init__(self, address=0x20, bus=1):
        """Initialize LCD

        Args:
            address: I2C address (default 0x20)
            bus: I2C bus number (default 1)
        """

    def write(self, text: str):
        """Write text to LCD"""

    def clear(self):
        """Clear LCD display"""

    def set_cursor(self, row: int, col: int):
        """Set cursor position

        Args:
            row: Row (0 or 1)
            col: Column (0-15)
        """

    def backlight(self, state: bool):
        """Control backlight

        Args:
            state: True for on, False for off
        """

    def create_char(self, location: int, pattern: list):
        """Create custom character

        Args:
            location: Character position (0-7)
            pattern: 8-byte list defining character
        """
```

### C API

#### lcd_driver.c

```c
// I2C communication
void mcp_write(uint8_t reg, uint8_t value);
uint8_t mcp_read(uint8_t reg);

// LCD operations
void lcd_init();
void lcd_command(uint8_t command);
void lcd_write_char(char c);
void lcd_write_text(const char *text);
void lcd_set_backlight(uint8_t state);

// Low-level operations
void lcd_pulse_enable();
void lcd_send_nibble(uint8_t data);
void lcd_send_byte(uint8_t data, uint8_t rs);
```

**Constants:**
```c
#define I2C_ADDRESS 0x20
#define IODIR 0x00
#define GPIO  0x09
#define OLAT  0x0A

#define RS  (1 << 1)
#define E   (1 << 2)
#define D4  (1 << 3)
#define D5  (1 << 4)
#define D6  (1 << 5)
#define D7  (1 << 6)
#define BACKLIGHT (1 << 7)
```

---

## Common Data Structures

### Motor Control

```c
// Motor speed (PWM range)
#define PWM_MIN 0
#define PWM_MAX 2000

// Motor direction states
typedef struct {
    int in1;
    int in2;
    int in3;
    int in4;
    int ena;
    int enb;
} MotorState;
```

### LCD Display

```c
// LCD configuration
typedef struct {
    uint8_t address;
    int i2c_fd;
    uint8_t backlight_state;
    uint8_t cursor_row;
    uint8_t cursor_col;
} LCDConfig;
```

### Camera Streaming

```c
// Frame buffer
typedef struct {
    unsigned char *data;
    size_t size;
    int width;
    int height;
} FrameBuffer;
```

---

## Error Codes

### Motor Control
- `0` - Success
- `-1` - GPIO initialization failed
- `-2` - FIFO creation failed
- `-3` - Invalid command

### LCD Display
- `0` - Success
- `-1` - I2C device not found
- `-2` - I2C communication error
- `-3` - Invalid address

### Camera
- `0` - Success
- `-1` - Camera not detected
- `-2` - Failed to open device
- `-3` - Format not supported

---

## Configuration Files

### Default Paths

```bash
# FIFO
/tmp/motor_control_fifo

# I2C Device
/dev/i2c-1

# Video Device
/dev/video0

# Log Files
/var/log/robot/motor.log
/var/log/robot/camera.log
```

### Environment Variables

```bash
# Camera settings
export CAMERA_RESOLUTION="1920x1080"
export CAMERA_ROTATION="-90"
export CAMERA_PORT="8000"

# Motor settings
export MOTOR_PWM_RANGE="2000"
export MOTOR_SPEED_FORWARD="500"

# LCD settings
export LCD_I2C_ADDRESS="0x20"
export LCD_I2C_BUS="1"
```

---

## Integration Examples

### Complete Robot Control

```python
from camera import camera_stream
from motor import writer
from lcd import lcd_driver
import threading

# Initialize components
lcd = lcd_driver.LCD()
camera_thread = threading.Thread(target=camera_stream.start)

# Start camera
camera_thread.start()

# Display status
lcd.write("Robot Ready")

# Control motors
writer.write_command('fw')
time.sleep(2)
writer.write_command('st')

# Update LCD
lcd.clear()
lcd.write("Motor Stopped")
```

### Sensor Integration

```python
def main_loop():
    while True:
        # Read distance sensor
        distance = read_ultrasonic()

        # Update LCD
        lcd.clear()
        lcd.write(f"Dist: {distance}cm")

        # Auto-brake
        if distance < 20:
            writer.write_command('st')
            lcd.set_cursor(1, 0)
            lcd.write("OBSTACLE!")

        time.sleep(0.1)
```
