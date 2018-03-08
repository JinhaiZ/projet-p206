from flask import Flask, render_template, Response
from .irCamera import IRCamera
import time

app = Flask(__name__)

nb_detection = 0

@app.route('/')
def index():
    return render_template('index.html')

def gen(camera):
    while True:
        frame, nb_detection = camera.get_frame()
        print("nb_detection: {}".format(nb_detection))
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n\r\n')

@app.route('/video_feed')
def video_feed():
    return Response(gen(IRCamera(test=True, file_name="./backgroundSubtraction/test.avi")),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/get_nb_dectection')
def get_nb_dectection():
    return str(nb_detection)

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)