import json
import time
import threading
import serial
from flask import Flask, render_template
from flask_socketio import SocketIO

app = Flask(__name__, static_folder='.', template_folder='.')
app.config['SECRET_KEY'] = 'maze_game_secret'
socketio = SocketIO(app, cors_allowed_origins="*")

# ==================== PORT CONFIGURATION ====================
MANUAL_PORT = 'COM12'  # Set to your active serial port
BAUD_RATE = 9600
# ============================================================

serial_conn = None
running = True

def serial_reader_thread():
    global serial_conn, running

    while running:
        if serial_conn is None or not serial_conn.is_open:
            try:
                print(f"[SERIAL] Connecting to {MANUAL_PORT}...")
                serial_conn = serial.Serial(MANUAL_PORT, BAUD_RATE, timeout=1)
                time.sleep(1)
                print("[SERIAL] Connected successfully!")
                socketio.emit('status', {'connected': True, 'port': MANUAL_PORT})
            except Exception as e:
                socketio.emit('status', {'connected': False, 'error': 'Access Denied'})
                time.sleep(2)
                continue

        try:
            if serial_conn.in_waiting > 0:
                line = serial_conn.readline().decode('utf-8', errors='ignore').strip()
                if line and line.startswith('{') and line.endswith('}'):
                    try:
                        data = json.loads(line)
                        socketio.emit('sensor_data', data)
                    except json.JSONDecodeError:
                        pass
        except Exception as e:
            print("Connection error:", e)
            socketio.emit('status', {
                'connected': False,
                'error': str(e)
            })
            time.sleep(2)
            continue

        time.sleep(0.005) # Tiny sleep for optimal gaming throughput

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    thread = threading.Thread(target=serial_reader_thread, daemon=True)
    thread.start()
    print("[SERVER] Maze Game Server active on http://localhost:5000")
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)