#!/bin/bash
# Run all tests for the Raspberry Pi robot project

set -e  # Exit on error

echo "=========================================="
echo "Running Raspberry Pi Robot Tests"
echo "=========================================="

# Check if pytest is installed
if ! command -v pytest &> /dev/null; then
    echo "pytest not found. Installing..."
    pip install pytest pytest-cov
fi

# Run Python tests
echo ""
echo "Running Python tests..."
echo "------------------------"

# Camera tests
echo ""
echo "Camera module tests:"
python -m pytest camera/tests/ -v --tb=short

# Motor tests
echo ""
echo "Motor module tests:"
python -m pytest motor/tests/ -v --tb=short

# LCD tests
echo ""
echo "LCD module tests:"
python -m pytest lcd/tests/ -v --tb=short

# Run with coverage
echo ""
echo "Running tests with coverage..."
echo "-------------------------------"
python -m pytest camera/tests/ motor/tests/ lcd/tests/ \
    --cov=camera/python --cov=motor/python --cov=lcd/python \
    --cov-report=term-missing \
    --cov-report=html

echo ""
echo "=========================================="
echo "Tests complete!"
echo "=========================================="
echo ""
echo "Coverage report saved to htmlcov/index.html"
