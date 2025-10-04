# Camera Module Documentation

## Overview

The camera module provides video streaming capabilities using three different implementations:

1. **Python (Flask + PiCamera)** - Simplest, high-level API
2. **C (V4L2)** - Low-level, direct hardware access
3. **C++ (OpenCV + Poco)** - Feature-rich with frame processing

## Python Implementation

### Flask + PiCamera (Recommended for Beginners)

**File:** `camera/python/camera_stream.py`

```python
# Run the camera stream server
python camera/python/camera_stream.py
```

**Access stream:**
```
http://<raspberry-pi-ip>:8000/video_feed
```

**Features:**
- 1920x1080 resolution
- Adjustable rotation (-90°)
- MJPEG streaming over HTTP
- Simple Flask web server

### PiCamera2 Implementation

**Files:**
- `camera/python/pi3_camera.py`
- `camera/python/hd_camera.py`

For newer Raspberry Pi OS with PiCamera2 library.

## C Implementation

### V4L2 Direct Access

**File:** `camera/c/c_stream.c`

```bash
# Compile
gcc -o camera/build/c_stream camera/c/c_stream.c -lpthread

# Run
./camera/build/c_stream
```

**Features:**
- Direct V4L2 (Video4Linux2) API
- Multithreaded frame capture
- HTTP MJPEG streaming on port 8000
- 640x480 resolution

**Configuration:**
```c
#define PORT 8000
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
```

## C++ Implementation

### OpenCV + Poco Framework

**File:** `camera/cpp/stream.cpp`

```bash
# Compile
g++ -o camera/build/stream camera/cpp/stream.cpp \
    -lopencv_core -lopencv_videoio -lopencv_imgcodecs \
    -lPocoNet -lPocoFoundation -lpthread

# Run
./camera/build/stream
```

**Features:**
- OpenCV for image processing
- Poco for HTTP server
- Frame rotation (90° counter-clockwise)
- Multi-client support
- Port 8000

**Frame Processing:**
```cpp
// Rotate frame counter-clockwise 90°
transpose(frameToSend, frameToSend);
flip(frameToSend, frameToSend, 0);
```

## Choosing an Implementation

| Implementation | Pros | Cons | Use Case |
|---------------|------|------|----------|
| **Python/Flask** | Simple, high-level, quick setup | Slower performance | Prototyping, learning |
| **C/V4L2** | Fast, low overhead, direct control | Complex, manual memory mgmt | Performance-critical |
| **C++/OpenCV** | Feature-rich, image processing | Heavy dependencies | Computer vision projects |

## Troubleshooting

### Camera Not Detected
```bash
# Check camera connection
vcgencmd get_camera

# Should show: supported=1 detected=1

# List video devices
ls -l /dev/video*
```

### Enable Legacy Camera Support
```bash
sudo raspi-config
# Interface Options → Legacy Camera → Enable
```

### Permission Issues
```bash
# Add user to video group
sudo usermod -a -G video $USER
```

### Test Camera
```bash
# Capture test image
libcamera-still -o test.jpg

# Or with legacy
raspistill -o test.jpg
```

## API Reference

### Python (PiCamera)
```python
camera.resolution = (1920, 1080)
camera.rotation = -90
camera.capture(stream, format='jpeg', use_video_port=True)
```

### C (V4L2)
```c
// Set format
struct v4l2_format fmt;
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
fmt.fmt.pix.width = FRAME_WIDTH;
fmt.fmt.pix.height = FRAME_HEIGHT;
ioctl(videoDevice, VIDIOC_S_FMT, &fmt);
```

### C++ (OpenCV)
```cpp
VideoCapture camera(0);
Mat frame;
camera.read(frame);
```
