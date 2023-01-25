from flask import Flask
from flask import request
import json

app = Flask(__name__)

@app.route("/")
def sensorlog():
    return "<p>Sensor log app recorder at /log</p>"

@app.route('/log', methods=['POST'])
def log():

    data = json.loads(request.get_data())
    print(data)
    with open("sensorlog.log", 'a') as f:
        f.write(json.dumps(data))
        
    return 'success'
