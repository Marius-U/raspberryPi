from picamera2 import Picamera2, Preview
from libcamera import Transform
import time

picam2 = Picamera2()

# Create a preview configuration with 90 degrees clockwise rotation
camera_config = picam2.create_preview_configuration(transform=Transform(rotation=90))
picam2.configure(camera_config)

# Start the preview using DRM (non-GUI) with desired position and size
picam2.start_preview(Preview.DRM, x=100, y=200, width=800, height=600)
picam2.start()

picam2.start()
time.sleep(2)
picam2.capture_file("test.jpg")

#camera.rotation = -90

#camera.start_preview()
#sleep(5)
#camera.stop_preview()
