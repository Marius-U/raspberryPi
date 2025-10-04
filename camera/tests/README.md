# Camera Module Tests

## Running Tests

### Python Tests
```bash
# Run all tests
python -m pytest camera/tests/

# Run specific test file
python camera/tests/test_camera_stream.py

# Run with coverage
python -m pytest camera/tests/ --cov=camera/python
```

### C/C++ Tests
```bash
# Compile and run C tests
cd camera/c
gcc -o test_camera c_stream.c -lpthread
./test_camera

# Compile and run C++ tests
cd camera/cpp
g++ -o test_stream stream.cpp -lopencv_core -lopencv_videoio -lopencv_imgcodecs -lPocoNet -lPocoFoundation
./test_stream
```

## Test Coverage

- `test_camera_stream.py`: Unit tests for camera streaming functions
- `test_integration.py`: Integration tests for HTTP streaming

## Hardware Requirements

Tests that require actual hardware (camera, GPIO) will be skipped in CI/CD environments.
Use mocking for unit tests when hardware is not available.
