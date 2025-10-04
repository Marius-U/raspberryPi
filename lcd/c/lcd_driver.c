#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

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

int i2c_fd;

void mcp_write(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg, value};
    write(i2c_fd, buffer, 2);
}

uint8_t mcp_read(uint8_t reg) {
    write(i2c_fd, &reg, 1);
    uint8_t value;
    read(i2c_fd, &value, 1);
    return value;
}

void lcd_pulse_enable() {
    mcp_write(GPIO, mcp_read(GPIO) | E);
    usleep(500);
    mcp_write(GPIO, mcp_read(GPIO) & ~E);
    usleep(500);
}

void lcd_send_nibble(uint8_t data) {
    uint8_t current = mcp_read(GPIO) & (RS | BACKLIGHT);
    uint8_t value = current | ((data << 3) & (D4 | D5 | D6 | D7));
    mcp_write(GPIO, value);
    lcd_pulse_enable();
}

void lcd_send_byte(uint8_t data, uint8_t rs) {
    if (rs) {
        mcp_write(GPIO, mcp_read(GPIO) | RS);
    } else {
        mcp_write(GPIO, mcp_read(GPIO) & ~RS);
    }
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_command(uint8_t command) {
    lcd_send_byte(command, 0);
    usleep(2000);
}

void lcd_write_char(char c) {
    lcd_send_byte(c, 1);
}

void lcd_init() {
    mcp_write(IODIR, 0x00);
    mcp_write(OLAT, 0x00);
    usleep(50000);
    lcd_send_nibble(0x03);
    usleep(5000);
    lcd_send_nibble(0x03);
    usleep(5000);
    lcd_send_nibble(0x03);
    usleep(5000);
    lcd_send_nibble(0x02);
    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    usleep(2000);
}

void lcd_set_backlight(uint8_t state) {
    if (state) {
        mcp_write(GPIO, mcp_read(GPIO) | BACKLIGHT);
    } else {
        mcp_write(GPIO, mcp_read(GPIO) & ~BACKLIGHT);
    }
}

void lcd_write_text(const char *text) {
    while (*text) {
        lcd_write_char(*text++);
    }
}

int main() {
    i2c_fd = open("/dev/i2c-1", O_RDWR);
    if (i2c_fd < 0) {
        perror("Failed to open I2C device");
        return 1;
    }
    if (ioctl(i2c_fd, I2C_SLAVE, I2C_ADDRESS) < 0) {
        perror("Failed to set I2C address");
        return 1;
    }
    printf("Initializing LCD...\n");
    lcd_init();
    printf("Turning backlight ON\n");
    lcd_set_backlight(1);
    printf("Displaying text...\n");
    lcd_write_text("Hello, World!");
    close(i2c_fd);
    return 0;
}
