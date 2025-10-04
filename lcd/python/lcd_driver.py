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
BACKLIGHT = 7  # Backlight (GP7)

# MCP23008 Register Addresses
IODIR = 0x00  # Direction register (all output)
GPIO = 0x09  # Output register
OLAT = 0x0A  # Output latch register

def mcp_write(register, value):
    bus.write_byte_data(I2C_ADDRESS, register, value)

def mcp_read(register):
    return bus.read_byte_data(I2C_ADDRESS, register)

def lcd_pulse_enable():
    mcp_write(GPIO, mcp_read(GPIO) | (1 << E))
    time.sleep(0.0005)
    mcp_write(GPIO, mcp_read(GPIO) & ~(1 << E))
    time.sleep(0.0005)

def lcd_send_nibble(data):
    current = mcp_read(GPIO) & 0x87  # Preserve RS and backlight states
    data = (data << 3) & 0x78  # Shift to match D4-D7
    mcp_write(GPIO, current | data)
    lcd_pulse_enable()

def lcd_send_byte(data, rs):
    if rs:
        mcp_write(GPIO, mcp_read(GPIO) | (1 << RS))
    else:
        mcp_write(GPIO, mcp_read(GPIO) & ~(1 << RS))
    lcd_send_nibble(data >> 4)
    lcd_send_nibble(data & 0x0F)

def lcd_command(command):
    lcd_send_byte(command, rs=0)
    time.sleep(0.002)

def lcd_write_char(char):
    lcd_send_byte(ord(char), rs=1)

def lcd_init():
    mcp_write(IODIR, 0x00)  # Set all GPIOs as outputs
    mcp_write(OLAT, 0x00)  # Clear outputs
    time.sleep(0.05)
    lcd_send_nibble(0x03)
    time.sleep(0.005)
    lcd_send_nibble(0x03)
    time.sleep(0.005)
    lcd_send_nibble(0x03)
    time.sleep(0.005)
    lcd_send_nibble(0x02)  # Set to 4-bit mode
    lcd_command(0x28)  # 4-bit, 2-line, 5x8 dots
    lcd_command(0x0C)  # Display ON, cursor OFF
    lcd_command(0x06)  # Entry mode set
    lcd_command(0x01)  # Clear display
    time.sleep(0.002)

def lcd_set_backlight(state):
    if state:
        mcp_write(GPIO, mcp_read(GPIO) | (1 << BACKLIGHT))
    else:
        mcp_write(GPIO, mcp_read(GPIO) & ~(1 << BACKLIGHT))

def lcd_write_text(text):
    for char in text:
        lcd_write_char(char)

if __name__ == "__main__":
    print("Initializing LCD...")
    lcd_init()
    print("Turning backlight ON")
    lcd_set_backlight(1)
    print("Displaying text...")
    lcd_write_text("Hello, World!")
