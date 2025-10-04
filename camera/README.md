# Camera Module

Video streaming component for Raspberry Pi robot with multiple implementation options.

## Implementations

### Python (Recommended for Beginners)

- **camera_stream.py** - Flask + PiCamera, full HD streaming
- **hd_camera.py** - HD camera setup
- **pi3_camera.py** - PiCamera2 implementation
- **test.py** - Camera testing utilities

### C (Performance-Critical)

- **c_stream.c** - V4L2 direct access, low latency streaming

### C++ (Feature-Rich)

- **stream.cpp** - OpenCV + Poco, with image processing

## Quick Start

### Python
```bash
# Install dependencies
pip install -r ../requirements.txt

# Run camera stream
python python/camera_stream.py

# Access at: http://<pi-ip>:8000/video_feed
```

### C
```bash
# Build
make camera-c

# Run
./build/c_stream
```

### C++
```bash
# Build
make camera-cpp

# Run
./build/stream
```

## Features

- MJPEG HTTP streaming
- Multi-client support
- Adjustable resolution and frame rate
- Image rotation support

## Documentation

See [docs/camera_usage.md](../docs/camera_usage.md) for detailed usage information.

## Testing

```bash
# Run tests
python -m pytest tests/
```
