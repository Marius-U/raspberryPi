import os
import sys
import termios
import tty
import time

FIFO_PATH = '/tmp/motor_control_fifo'

def send_command(command):
    """Send a command to the FIFO."""
    try:
        # Write the command to the FIFO
        with open(FIFO_PATH, 'w') as fifo:
            fifo.write(command + '\n')
    except Exception as e:
        print(f"Failed to send command: {e}")

def getch():
    """Get a single character from standard input."""
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)
        if ch == '\x1b':  # Escape character
            ch += sys.stdin.read(2)  # Read the next two characters
        return ch
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

def main():
    print("Press UP, DOWN, LEFT, or RIGHT to control the motors. Press ESC to exit.")

    try:
        while True:
            key = getch()  # Get a single keypress
            if key == '\x1b[A':  # Up arrow
                send_command('forward')
            elif key == '\x1b[B':  # Down arrow
                send_command('backward')
            elif key == '\x1b[D':  # Left arrow
                send_command('left')
            elif key == '\x1b[C':  # Right arrow
                send_command('right')
            elif key == '\x1b':  # Escape key
                send_command('stop')  # Ensure motors stop when exiting
                break
            time.sleep(0.1)  # Prevent spamming commands
    except KeyboardInterrupt:
        print("Exiting...")

if __name__ == '__main__':
    main()
