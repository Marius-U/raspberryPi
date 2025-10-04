import os
import time

FIFO_PATH = "/tmp/motor_control_fifo"

def write_command(command):
    if not os.path.exists(FIFO_PATH):
        print(f"FIFO {FIFO_PATH} does not exist.")
        return

    try:
        with open(FIFO_PATH, 'w') as fifo:
            fifo.write(command)
            print(f"Command '{command}' written to FIFO.")
    except Exception as e:
        print(f"Failed to write to FIFO: {e}")

if __name__ == "__main__":
    while True:
        print("Enter command (forward, backward, stop):")
        command = input().strip().lower()

        if command in ["forward", "backward", "stop"]:
            write_command(command)
        else:
            print("Invalid command. Please enter 'forward', 'backward', or 'stop'.")

        time.sleep(1)  # Small delay before accepting the next command
