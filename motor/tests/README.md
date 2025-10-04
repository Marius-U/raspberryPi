# Motor Control Tests

## Running Tests

### Python Tests
```bash
# Run all tests
python -m pytest motor/tests/

# Run specific test file
python motor/tests/test_motor_control.py
python motor/tests/test_fifo_comm.py

# Run with coverage
python -m pytest motor/tests/ --cov=motor/python
```

### C Tests
```bash
# Compile motor control programs
cd motor/c
gcc -o moto_fifo moto_fifo.c -lwiringPi
gcc -o motoFifoKey motoFifoKey.c -lwiringPi

# Test FIFO communication
# Terminal 1: Start motor control listener
./moto_fifo

# Terminal 2: Send commands
echo "fw" > /tmp/motor_control_fifo  # Forward
echo "bw" > /tmp/motor_control_fifo  # Backward
echo "st" > /tmp/motor_control_fifo  # Stop
```

## Test Coverage

- `test_motor_control.py`: Unit tests for motor control functions
- `test_fifo_comm.py`: Tests for FIFO-based command communication

## Hardware Requirements

Motor control tests require:
- L298N H-Bridge motor driver
- GPIO pins configured as per README.md
- WiringPi library installed

Tests will use GPIO mocking when running without hardware.
