import cv2
from flask import Flask, Response

# Initialize the Flask application
app = Flask(__name__)

# Open the camera using OpenCV
camera = cv2.VideoCapture(0)  # Change 0 to the index of your camera if different

def generate_frames():
    while True:
        # Capture frame-by-frame
        success, frame = camera.read()
        if not success:
            break
        else:
            # Encode the frame as JPEG
            ret, buffer = cv2.imencode('.jpg', frame)
            if not ret:
                break
            # Convert the frame to bytes
            frame_bytes = buffer.tobytes()
            # Yield the frame in byte format
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

@app.route('/')
def index():
    # Return the streaming content
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    # Run the Flask app
    app.run(host='0.0.0.0', debug=True)
