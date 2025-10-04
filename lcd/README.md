# LCD Display Module

16x2 character LCD control using MCP23008 I2C I/O expander.

## Hardware

**I2C Configuration:**
- MCP23008 Address: 0x20
- SDA → GPIO 2
- SCL → GPIO 3

**LCD Connection via MCP23008:**
- RS → MCP Pin 1
- E → MCP Pin 2
- D4-D7 → MCP Pins 3-6
- Backlight → MCP Pin 7

## Implementations

### Python

- **lcd_driver.py** - Main LCD driver class
- **lcd_test.py** - LCD testing utilities
- **find.py** - I2C device scanner

### C

- **lcd_driver.c** - Direct I2C LCD control

## Quick Start

### Enable I2C

```bash
sudo raspi-config
# Interface Options → I2C → Enable

# Verify
i2cdetect -y 1
```

### Python Usage

```python
from lcd.python.lcd_driver import LCD

lcd = LCD()
lcd.write("Hello, World!")
lcd.backlight(True)
```

### C Usage

```bash
# Build
make lcd

# Run
./build/lcd_driver
```

## Features

- 16x2 character display
- Backlight control
- Custom character support
- I2C communication at 0x20

## Documentation

See [docs/lcd_display.md](../docs/lcd_display.md) for detailed information.

## Testing

```bash
# Run tests
python -m pytest tests/

# Test I2C device
python python/find.py
```
