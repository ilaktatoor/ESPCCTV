from flask import Flask, Response, request
import cv2
import numpy as np

app = Flask(__name__)

# Initialize a dictionary to hold the most recent frame for each camera
cameras = {}

@app.route('/video-frame/<cam_id>', methods=['POST'])
def handle_video_frame(cam_id):
    global cameras

    # Receive the image data from the POST request
    image_data = request.data

    # Convert the image to a numpy array
    np_img = np.frombuffer(image_data, np.uint8)

    # Decode the image into an OpenCV format
    img = cv2.imdecode(np_img, cv2.IMREAD_COLOR)

    if img is not None:
        # Store the frame for the specific camera
        cameras[cam_id] = img

    return "Frame received", 200

def generate(cam_id):
    global cameras
    while True:
        if cam_id in cameras and cameras[cam_id] is not None:
            # Encode the current frame to JPEG
            _, jpeg = cv2.imencode('.jpg', cameras[cam_id])
            frame = jpeg.tobytes()

            # Yield the frame as part of the MJPEG stream
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

@app.route('/video/<cam_id>')
def video(cam_id):
    # Serve the MJPEG stream for the specific camera
    if cam_id in cameras:
        return Response(generate(cam_id), mimetype='multipart/x-mixed-replace; boundary=frame')
    else:
        return "Camera not found", 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5050, threaded=True)

