from flask import Flask, Response
from picamera import PiCamera
import time
import io

app = Flask(__name__)

# Initialize PiCamera object
camera = PiCamera()

# Set resolution (you can adjust this as needed)
camera.resolution = (1920, 1080)

# Set up camera rotation if needed
camera.rotation = -90  # Adjust as needed

# Define a generator function to capture frames from the camera
def generate_frames():
    # Start the camera preview
    camera.start_preview()
    time.sleep(2)  # Allow camera to warm up
    
    # Create an in-memory stream for buffering frames
    stream = io.BytesIO()
    
    # Capture frames continuously
    while True:
        # Capture a frame and write it to the in-memory stream
        camera.capture(stream, format='jpeg', use_video_port=True)
        
        # Move the stream pointer to the beginning
        stream.seek(0)
        
        # Read the stream and yield the frame to the Flask response
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + stream.read() + b'\r\n')
        
        # Reset the stream for the next frame
        stream.seek(0)
        stream.truncate()
        
        time.sleep(0.1)  # Adjust the delay as needed to control the frame rate

# Define a route to serve the camera feed
@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    try:
        # Run the Flask app
        app.run(host='0.0.0.0', port=8000, debug=False)
    finally:
        # Stop the camera preview when exiting the application
        camera.stop_preview()