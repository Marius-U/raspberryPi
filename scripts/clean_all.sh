#!/bin/bash
# Clean all build artifacts

echo "Cleaning build artifacts..."

# Remove build directories
rm -rf camera/build/*
rm -rf motor/build/*
rm -rf lcd/build/*

# Remove Python cache
find . -type d -name "__pycache__" -exec rm -rf {} + 2>/dev/null
find . -type f -name "*.pyc" -delete 2>/dev/null
find . -type f -name "*.pyo" -delete 2>/dev/null

# Remove test artifacts
rm -rf .pytest_cache
rm -rf htmlcov
rm -f .coverage

# Remove temporary files
rm -f /tmp/motor_control_fifo 2>/dev/null
rm -f camera/test.jpg 2>/dev/null

echo "Clean complete!"
