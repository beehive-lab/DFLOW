import logging
import threading

import flask
from flask import jsonify, render_template
from werkzeug.serving import make_server

from client.communication.messages import SensorDataKey
from client.communication.on_board import OnBoard

app = flask.Flask(__name__, )
app.logger.disabled = True
log = logging.getLogger('werkzeug')
log.disabled = True
log.disabled = True

on_board: OnBoard


@app.route('/', methods=['GET'])
def root():
    return render_template("index.html")


@app.route('/api/v1/live-sensor-data/<sensor_data_key>', methods=['GET'])
def live_sensor_data(sensor_data_key):
    latest_sensor_data = on_board.get_latest_sensor_data(sensor_data_key)
    if latest_sensor_data:
        data_timestamp = int(latest_sensor_data[0])
        data_val = float(latest_sensor_data[1])
    else:
        data_timestamp = None
        data_val = None

    return jsonify(
        {
            'key': sensor_data_key,
            'timestamp': data_timestamp,
            'value': data_val
        }
    )


@app.route('/api/v1/live-sensor-data/', methods=['GET'])
def all_live_sensor_data():
    all_latest_data = []
    for sensor_data_key in SensorDataKey:
        latest_sensor_data = on_board.get_latest_sensor_data(
            sensor_data_key
        )
        if latest_sensor_data:
            data_timestamp = int(latest_sensor_data[0])
            data_val = float(latest_sensor_data[1])
            all_latest_data.append(
                {
                    'key': str(sensor_data_key),
                    'timestamp': data_timestamp,
                    'value': data_val
                }
            )
    return jsonify(all_latest_data)


@app.route('/api/v1/sensor-data-keys', methods=['GET'])
def sensor_data_keys():
    return jsonify(
        [str(key) for key in SensorDataKey]
    )


class ServerThread(threading.Thread):

    def __init__(self, app, host, port):
        threading.Thread.__init__(self)
        self.srv = make_server(host, port, app)
        self.ctx = app.app_context()
        self.ctx.push()

    def run(self):
        self.srv.serve_forever()

    def shutdown(self):
        self.srv.shutdown()


server: ServerThread


def start_server(on_board_for_server: OnBoard, api_host: str, api_port: int):
    global server
    global on_board
    on_board = on_board_for_server
    server = ServerThread(app, api_host, api_port)
    server.start()


def stop_server():
    global server
    server.shutdown()
