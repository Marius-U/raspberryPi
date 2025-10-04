# Raspberry Pi Robot Project Makefile

# Compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -O2
CXXFLAGS = -Wall -O2

# Libraries
CAMERA_C_LIBS = -lpthread
CAMERA_CPP_LIBS = -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lPocoNet -lPocoFoundation -lpthread
MOTOR_LIBS = -lwiringPi
LCD_LIBS =

# Directories
CAMERA_BUILD = camera/build
MOTOR_BUILD = motor/build
LCD_BUILD = lcd/build

# Targets
CAMERA_C_TARGET = $(CAMERA_BUILD)/c_stream
CAMERA_CPP_TARGET = $(CAMERA_BUILD)/stream
MOTOR_FIFO_TARGET = $(MOTOR_BUILD)/moto_fifo
MOTOR_FIFO_KEY_TARGET = $(MOTOR_BUILD)/motoFifoKey
MOTOR_TARGET = $(MOTOR_BUILD)/moto
MOTOR_H_TARGET = $(MOTOR_BUILD)/moto_h
LCD_TARGET = $(LCD_BUILD)/lcd_driver

# Default target
.PHONY: all
all: camera motor lcd

# Camera targets
.PHONY: camera camera-c camera-cpp
camera: camera-c camera-cpp

camera-c: $(CAMERA_C_TARGET)

camera-cpp: $(CAMERA_CPP_TARGET)

$(CAMERA_C_TARGET): camera/c/c_stream.c
	@mkdir -p $(CAMERA_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(CAMERA_C_LIBS)
	@echo "Built camera C implementation: $@"

$(CAMERA_CPP_TARGET): camera/cpp/stream.cpp
	@mkdir -p $(CAMERA_BUILD)
	$(CXX) $(CXXFLAGS) -o $@ $< $(CAMERA_CPP_LIBS)
	@echo "Built camera C++ implementation: $@"

# Motor targets
.PHONY: motor
motor: $(MOTOR_FIFO_TARGET) $(MOTOR_FIFO_KEY_TARGET) $(MOTOR_TARGET) $(MOTOR_H_TARGET)

$(MOTOR_FIFO_TARGET): motor/c/moto_fifo.c
	@mkdir -p $(MOTOR_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(MOTOR_LIBS)
	@echo "Built motor FIFO controller: $@"

$(MOTOR_FIFO_KEY_TARGET): motor/c/motoFifoKey.c
	@mkdir -p $(MOTOR_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(MOTOR_LIBS)
	@echo "Built motor FIFO key controller: $@"

$(MOTOR_TARGET): motor/c/moto.c
	@mkdir -p $(MOTOR_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(MOTOR_LIBS)
	@echo "Built motor basic controller: $@"

$(MOTOR_H_TARGET): motor/c/moto_h.c
	@mkdir -p $(MOTOR_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(MOTOR_LIBS)
	@echo "Built motor H-bridge controller: $@"

# LCD targets
.PHONY: lcd
lcd: $(LCD_TARGET)

$(LCD_TARGET): lcd/c/lcd_driver.c
	@mkdir -p $(LCD_BUILD)
	$(CC) $(CFLAGS) -o $@ $< $(LCD_LIBS)
	@echo "Built LCD driver: $@"

# Clean targets
.PHONY: clean clean-camera clean-motor clean-lcd
clean: clean-camera clean-motor clean-lcd
	@echo "Cleaned all build artifacts"

clean-camera:
	rm -rf $(CAMERA_BUILD)/*
	@echo "Cleaned camera build"

clean-motor:
	rm -rf $(MOTOR_BUILD)/*
	@echo "Cleaned motor build"

clean-lcd:
	rm -rf $(LCD_BUILD)/*
	@echo "Cleaned LCD build"

# Test targets
.PHONY: test test-python
test: test-python

test-python:
	@echo "Running Python tests..."
	python -m pytest camera/tests/ motor/tests/ lcd/tests/ -v

# Install dependencies
.PHONY: install-deps
install-deps:
	@echo "Installing system dependencies..."
	sudo apt-get update
	sudo apt-get install -y gcc g++ make
	sudo apt-get install -y wiringpi i2c-tools
	sudo apt-get install -y libopencv-dev libpoco-dev
	@echo "Installing Python dependencies..."
	pip install -r requirements.txt

# Help
.PHONY: help
help:
	@echo "Raspberry Pi Robot Project Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build all components (default)"
	@echo "  camera        - Build camera modules (C and C++)"
	@echo "  camera-c      - Build C camera implementation"
	@echo "  camera-cpp    - Build C++ camera implementation"
	@echo "  motor         - Build motor control modules"
	@echo "  lcd           - Build LCD display module"
	@echo "  test          - Run all tests"
	@echo "  test-python   - Run Python tests"
	@echo "  clean         - Clean all build artifacts"
	@echo "  install-deps  - Install system and Python dependencies"
	@echo "  help          - Show this help message"
