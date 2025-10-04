# Motor Control Module

DC motor control for Raspberry Pi robot using L298N H-bridge driver.

## Hardware

**L298N Connections:**
- IN1 → GPIO 22 (Motor 1 Direction)
- IN2 → GPIO 23 (Motor 1 Direction)
- IN3 → GPIO 24 (Motor 2 Direction)
- IN4 → GPIO 25 (Motor 2 Direction)
- ENA → GPIO 12 (Motor 1 PWM Speed)
- ENB → GPIO 13 (Motor 2 PWM Speed)

## Implementations

### Python

- **p_moto.py** - Basic GPIO motor control
- **writer.py** - FIFO command writer
- **writingKey.py** - Keyboard-based control

### C

- **moto_fifo.c** - FIFO-based motor controller (recommended)
- **motoFifoKey.c** - FIFO with keyboard input
- **moto.c** - Basic motor control
- **moto_h.c** - H-bridge direct control

## Quick Start

### Start Motor Controller (C)

```bash
# Build
make motor

# Run FIFO controller
./build/moto_fifo &
```

### Send Commands

```bash
# Forward
echo "fw" > /tmp/motor_control_fifo

# Backward
echo "bw" > /tmp/motor_control_fifo

# Stop
echo "st" > /tmp/motor_control_fifo
```

### Python Control

```python
from motor.python import writer

writer.write_command('fw')  # Forward
writer.write_command('st')  # Stop
```

## Commands

- `fw` - Move forward
- `bw` - Move backward
- `st` - Stop motors

## Documentation

See [docs/motor_control.md](../docs/motor_control.md) for detailed information.

## Testing

```bash
# Run tests
python -m pytest tests/
```
