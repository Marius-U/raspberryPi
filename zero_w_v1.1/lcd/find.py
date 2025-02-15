import time
from smbus2 import SMBus

# MCP23008 I2C address (default is 0x20)
I2C_ADDRESS = 0x20
bus = SMBus(1)  # Use I2C bus 1

# MCP23008 GPIO Pin Mapping
RS = 1  # Register Select (GP1)
E = 2   # Enable (GP2)
D4 = 3  # Data 4 (GP3)
D5 = 4  # Data 5 (GP4)
D6 = 5  # Data 6 (GP5)
D7 = 6  # Data 7 (GP6)

# MCP23008 Register Addresses
IODIR = 0x00  # Direction register (all output)
GPIO = 0x09  # Output register
OLAT = 0x0A  # Output latch register

def mcp_write(register, value):
    bus.write_byte_data(I2C_ADDRESS, register, value)

def mcp_read(register):
    return bus.read_byte_data(I2C_ADDRESS, register)

def test_gpio_pins():
    print("Testing GPIO pins...")
    for pin in range(8):  # Iterate through GP0 to GP7
        print(f"Turning ON GP{pin}")
        mcp_write(GPIO, 1 << pin)
        time.sleep(1)
        print(f"Turning OFF GP{pin}")
        mcp_write(GPIO, 0)
        time.sleep(1)
    print("GPIO test complete. Observe which pin affects the backlight.")

if __name__ == "__main__":
    print("Initializing GPIO test...")
    mcp_write(IODIR, 0x00)  # Set all GPIOs as outputs
    mcp_write(OLAT, 0x00)  # Ensure known output state
    time.sleep(0.1)
    test_gpio_pins()
