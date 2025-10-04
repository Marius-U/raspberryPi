#!/bin/bash
# Build all components of the Raspberry Pi robot project

set -e  # Exit on error

echo "=========================================="
echo "Building Raspberry Pi Robot Project"
echo "=========================================="

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Check if we're on Raspberry Pi
if ! grep -q "Raspberry Pi" /proc/cpuinfo 2>/dev/null; then
    echo -e "${RED}Warning: Not running on Raspberry Pi${NC}"
    echo "Some components may fail to build without proper hardware libraries"
fi

# Create build directories
echo "Creating build directories..."
mkdir -p camera/build motor/build lcd/build

# Build camera modules
echo ""
echo "Building camera modules..."
echo "----------------------------"

# Camera C implementation
if gcc -Wall -O2 -o camera/build/c_stream camera/c/c_stream.c -lpthread 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Built camera C implementation"
else
    echo -e "${RED}✗${NC} Failed to build camera C implementation"
fi

# Camera C++ implementation
if command -v pkg-config &> /dev/null && pkg-config --exists opencv4; then
    if g++ -Wall -O2 -o camera/build/stream camera/cpp/stream.cpp \
        $(pkg-config --cflags --libs opencv4) -lPocoNet -lPocoFoundation -lpthread 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Built camera C++ implementation"
    else
        echo -e "${RED}✗${NC} Failed to build camera C++ implementation (missing Poco or OpenCV)"
    fi
else
    echo -e "${RED}✗${NC} OpenCV not found, skipping C++ camera build"
fi

# Build motor modules
echo ""
echo "Building motor control modules..."
echo "----------------------------------"

if command -v gpio &> /dev/null; then
    if gcc -Wall -O2 -o motor/build/moto_fifo motor/c/moto_fifo.c -lwiringPi 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Built motor FIFO controller"
    else
        echo -e "${RED}✗${NC} Failed to build motor FIFO controller"
    fi

    if gcc -Wall -O2 -o motor/build/motoFifoKey motor/c/motoFifoKey.c -lwiringPi 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Built motor FIFO key controller"
    else
        echo -e "${RED}✗${NC} Failed to build motor FIFO key controller"
    fi

    if gcc -Wall -O2 -o motor/build/moto motor/c/moto.c -lwiringPi 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Built motor basic controller"
    else
        echo -e "${RED}✗${NC} Failed to build motor basic controller"
    fi

    if gcc -Wall -O2 -o motor/build/moto_h motor/c/moto_h.c -lwiringPi 2>/dev/null; then
        echo -e "${GREEN}✓${NC} Built motor H-bridge controller"
    else
        echo -e "${RED}✗${NC} Failed to build motor H-bridge controller"
    fi
else
    echo -e "${RED}✗${NC} WiringPi not found, skipping motor builds"
fi

# Build LCD module
echo ""
echo "Building LCD display module..."
echo "-------------------------------"

if gcc -Wall -O2 -o lcd/build/lcd_driver lcd/c/lcd_driver.c 2>/dev/null; then
    echo -e "${GREEN}✓${NC} Built LCD driver"
else
    echo -e "${RED}✗${NC} Failed to build LCD driver"
fi

echo ""
echo "=========================================="
echo "Build complete!"
echo "=========================================="
echo ""
echo "Executables are in:"
echo "  - camera/build/"
echo "  - motor/build/"
echo "  - lcd/build/"
