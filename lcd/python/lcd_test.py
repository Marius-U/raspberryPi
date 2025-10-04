from smbus2 import SMBus
import time

# MCP23008 default I2C address (change if necessary)
I2C_ADDRESS = 0x20  # Default address for MCP23008

# Raspberry Pi uses I2C bus 1
bus = SMBus(1)

def scan_i2c():
    print("Scanning I2C bus...")
    devices = []
    for address in range(0x03, 0x78):
        try:
            bus.write_quick(address)
            devices.append(hex(address))
        except OSError:
            pass
    print("Found devices:", devices)

try:
    scan_i2c()
    
    # Initialize MCP23008 (IODIR register to set all as output for testing)
    bus.write_byte_data(I2C_ADDRESS, 0x00, 0x00)  # IODIR all output
    
    print("MCP23008 detected at address:", hex(I2C_ADDRESS))
    
    while True:
        # Blink all pins for testing
        bus.write_byte_data(I2C_ADDRESS, 0x09, 0xFF)  # Turn all pins ON
        print("LCD Backlight ON")
        time.sleep(1)
        
        bus.write_byte_data(I2C_ADDRESS, 0x09, 0x00)  # Turn all pins OFF
        print("LCD Backlight OFF")
        time.sleep(1)

except Exception as e:
    print("Error:", e)
