# LCD Display Documentation

## Overview

The LCD display module provides control of a 16x2 character LCD using:
- MCP23008 I2C I/O expander
- HD44780 compatible LCD controller
- 4-bit mode operation

## Hardware Configuration

### MCP23008 I2C Expander

**I2C Settings:**
- Address: `0x20`
- Bus: `/dev/i2c-1`
- SDA: GPIO 2
- SCL: GPIO 3

### LCD Pin Mapping

| MCP Pin | LCD Function | Bit Position |
|---------|--------------|--------------|
| GP1 | RS (Register Select) | Bit 1 |
| GP2 | E (Enable) | Bit 2 |
| GP3 | D4 (Data) | Bit 3 |
| GP4 | D5 (Data) | Bit 4 |
| GP5 | D6 (Data) | Bit 5 |
| GP6 | D7 (Data) | Bit 6 |
| GP7 | Backlight | Bit 7 |

## Python Implementation

### Basic LCD Control

**File:** `lcd/python/lcd_driver.py`

```python
from lcd_driver import LCD

# Initialize LCD
lcd = LCD()

# Display text
lcd.write("Hello, World!")

# Control backlight
lcd.backlight(True)   # On
lcd.backlight(False)  # Off

# Clear display
lcd.clear()
```

### Device Detection

**File:** `lcd/python/find.py`

```python
# Scan I2C bus for devices
python lcd/python/find.py
```

### Test Program

**File:** `lcd/python/lcd_test.py`

```python
# Run LCD test
python lcd/python/lcd_test.py
```

## C Implementation

### Direct I2C Control

**File:** `lcd/c/lcd_driver.c`

```bash
# Compile
gcc -o lcd/build/lcd_driver lcd/c/lcd_driver.c

# Run
./lcd/build/lcd_driver
```

**Functions:**
```c
void lcd_init();                    // Initialize LCD
void lcd_write_text(const char *);  // Display text
void lcd_write_char(char);          // Display single char
void lcd_command(uint8_t);          // Send command
void lcd_set_backlight(uint8_t);    // Control backlight
```

## LCD Commands

### Common HD44780 Commands

| Command | Hex | Description |
|---------|-----|-------------|
| Clear Display | 0x01 | Clear screen, home cursor |
| Return Home | 0x02 | Cursor to position 0,0 |
| Entry Mode | 0x06 | Increment cursor, no shift |
| Display ON | 0x0C | Display on, cursor off |
| Display OFF | 0x08 | Display off |
| Function Set | 0x28 | 4-bit mode, 2 lines, 5x8 font |
| Set DDRAM | 0x80 + addr | Set cursor position |

### Cursor Positions

**Line 1:** 0x00 to 0x0F (0-15)
**Line 2:** 0x40 to 0x4F (64-79)

```c
// Move to line 2, position 0
lcd_command(0x80 | 0x40);
```

## Initialization Sequence

The LCD requires a specific initialization sequence:

```c
// 1. Wait 50ms after power on
usleep(50000);

// 2. Send 0x03 three times
lcd_send_nibble(0x03);
usleep(5000);
lcd_send_nibble(0x03);
usleep(5000);
lcd_send_nibble(0x03);
usleep(5000);

// 3. Switch to 4-bit mode
lcd_send_nibble(0x02);

// 4. Configure display
lcd_command(0x28);  // 4-bit, 2 lines, 5x8 font
lcd_command(0x0C);  // Display on, cursor off
lcd_command(0x06);  // Auto-increment cursor
lcd_command(0x01);  // Clear display
usleep(2000);
```

## MCP23008 Registers

| Register | Address | Function |
|----------|---------|----------|
| IODIR | 0x00 | I/O direction (0=output) |
| GPIO | 0x09 | Port value |
| OLAT | 0x0A | Output latch |

```c
// Set all pins as output
mcp_write(IODIR, 0x00);

// Write to GPIO
mcp_write(GPIO, value);
```

## Usage Examples

### Display Static Text

```python
from lcd_driver import LCD

lcd = LCD()
lcd.write("Raspberry Pi")
lcd.set_cursor(1, 0)  # Line 2, position 0
lcd.write("Round Robot")
```

### Scrolling Text

```python
import time

text = "This is a scrolling message"
for i in range(len(text) - 15):
    lcd.clear()
    lcd.write(text[i:i+16])
    time.sleep(0.3)
```

### Display Sensor Data

```python
import time

while True:
    lcd.clear()
    lcd.write(f"Temp: {get_temp()}C")
    lcd.set_cursor(1, 0)
    lcd.write(f"Dist: {get_distance()}cm")
    time.sleep(1)
```

### Custom Characters

```python
# Define custom character (heart symbol)
heart = [
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000,
]

lcd.create_char(0, heart)
lcd.write_char(0)  # Display custom character
```

## Troubleshooting

### Enable I2C
```bash
sudo raspi-config
# Interface Options → I2C → Enable

# Verify I2C enabled
lsmod | grep i2c
```

### Detect I2C Device
```bash
# Install tools
sudo apt-get install i2c-tools

# Scan bus
i2cdetect -y 1

# Should show device at 0x20:
#      0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f
# 00:          -- -- -- -- -- -- -- -- -- -- -- -- --
# 10: -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
# 20: 20 -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
```

### LCD Not Working

1. **Check Connections:**
   - SDA → GPIO 2
   - SCL → GPIO 3
   - VCC → 5V
   - GND → GND

2. **Check Contrast:**
   - Adjust potentiometer on LCD backpack
   - Should see character blocks

3. **Check Address:**
   ```python
   # Try alternate addresses
   LCD(address=0x27)  # Some boards use 0x27
   ```

4. **Test I2C Communication:**
   ```bash
   i2cget -y 1 0x20 0x00
   ```

### Permission Issues
```bash
# Add user to i2c group
sudo usermod -a -G i2c $USER

# Logout and login for changes to take effect
```

## Advanced Features

### Multi-page Display
```python
pages = [
    ["Page 1 Line 1", "Page 1 Line 2"],
    ["Page 2 Line 1", "Page 2 Line 2"],
]

for page in pages:
    lcd.clear()
    lcd.write(page[0])
    lcd.set_cursor(1, 0)
    lcd.write(page[1])
    time.sleep(2)
```

### Progress Bar
```python
def show_progress(percent):
    bars = int(percent / 100 * 16)
    lcd.clear()
    lcd.write("Progress:")
    lcd.set_cursor(1, 0)
    lcd.write("█" * bars)
```
