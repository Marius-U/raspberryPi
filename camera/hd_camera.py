from flask import Flask, Response
import cv2

app = Flask(__name__)

# Initialize OpenCV VideoCapture object
cap = cv2.VideoCapture(0)  # 0 is the device index for /dev/video0

# Set resolution (you can adjust this as needed)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

# Define a generator function to capture frames from the webcam
def generate_frames():
    while True:
        # Read a frame from the webcam
        success, frame = cap.read()
        if not success:
            break

        # Convert the frame to JPEG format
        ret, buffer = cv2.imencode('.jpg', frame)
        if not ret:
            break

        # Yield the frame to the Flask response
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + buffer.tobytes() + b'\r\n')

# Define a route to serve the webcam feed
@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    try:
        # Run the Flask app
        app.run(host='0.0.0.0', port=8000, debug=False)
    finally:
        # Release the webcam when exiting the application
        cap.release()
