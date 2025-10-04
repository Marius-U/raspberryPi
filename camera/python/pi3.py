from flask import Flask, Response
from picamera import PiCamera
import io

app = Flask(__name__)

# Initialize PiCamera
try:
    camera = PiCamera()
    camera.resolution = (320, 240)  # Adjust the resolution as needed
except Exception as e:
    print("Error initializing PiCamera:", e)
    exit(1)

# Start the camera preview to warm up
camera.start_preview()
# Allow the camera to warm up
import time
time.sleep(2)

@app.route('/')
def stream():
    try:
        # Create an in-memory stream for image data
        stream = io.BytesIO()
        # Capture a frame from the camera and save it to the stream
        camera.capture(stream, format='jpeg', use_video_port=True)
        # Seek to the beginning of the stream
        stream.seek(0)
        # Return the image data as a Flask response with content type 'image/jpeg'
        return Response(stream, mimetype='image/jpeg')
    except Exception as e:
        print("Error capturing frame from PiCamera:", e)
        return "Error capturing frame from PiCamera", 500

if __name__ == '__main__':
    # Run the Flask app
    app.run(host='0.0.0.0', debug=True)
