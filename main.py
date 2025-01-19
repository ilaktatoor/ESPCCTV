from flask import Flask, Response, request
import cv2
import numpy as np

app = Flask(__name__)

# Initialize a variable to hold the most recent frame
current_frame = None

@app.route('/video-frame', methods=['POST'])
def handle_video_frame():
    global current_frame

    # Receive the image data from the POST request
    image_data = request.data

    # Convert the image to a numpy array
    np_img = np.frombuffer(image_data, np.uint8)

    # Decode the image into an OpenCV format
    img = cv2.imdecode(np_img, cv2.IMREAD_COLOR)

    if img is not None:
        # Store the frame
        current_frame = img

    return "Frame received", 200

def generate():
    global current_frame
    while True:
        if current_frame is not None:
            # Encode the current frame to JPEG
            _, jpeg = cv2.imencode('.jpg', current_frame)
            frame = jpeg.tobytes()

            # Yield the frame as part of the MJPEG stream
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

@app.route('/video')
def video():
    # Serve the MJPEG stream on the /video route
    return Response(generate(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5050, threaded=True)

