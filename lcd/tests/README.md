# LCD Display Tests

## Running Tests

### Python Tests
```bash
# Run all tests
python -m pytest lcd/tests/

# Run specific test file
python lcd/tests/test_lcd_driver.py
python lcd/tests/test_i2c_comm.py

# Run with coverage
python -m pytest lcd/tests/ --cov=lcd/python
```

### C Tests
```bash
# Compile LCD driver
cd lcd/c
gcc -o lcd_driver lcd_driver.c -li2c

# Run LCD driver test
./lcd_driver
```

### I2C Device Detection
```bash
# Detect I2C devices
i2cdetect -y 1

# Expected output should show device at address 0x20
```

## Test Coverage

- `test_lcd_driver.py`: Unit tests for LCD display functions
- `test_i2c_comm.py`: Tests for I2C communication with MCP23008

## Hardware Requirements

LCD tests require:
- 16x2 LCD display (HD44780 compatible)
- MCP23008 I2C I/O expander at address 0x20
- I2C enabled on Raspberry Pi (`/dev/i2c-1`)

Tests will use I2C mocking when running without hardware.
