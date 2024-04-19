import time
import datetime
import schedule
import requests
import threading
from flask import Flask, request

# update the pico ip address
pico_id = "192.168.88.84"
app = Flask(__name__)
job = 0

def convert_time_format(input_time):
    try:
        parsed_time = datetime.datetime.strptime(input_time, "%Y-%m-%d %H:%M:%S.%f")
        output_time = parsed_time.strftime("%H:%M")

        return output_time
    except ValueError:
        print("Invalid input time format.")
        return None

def dry_clean():
    print("DRY CLEAN")
    response = requests.get(f'http://{pico_id}/dc')
    return f'Sent command to Pico, Response: {response.text}', 200

def wet_clean():
    print("WET CLEAN")
    response = requests.get(f'http://{pico_id}/wc')
    return f'Sent command to Pico, Response: {response.text}', 200

def dry_wet_clean():
    print("DRY+WET CLEAN")
    response = requests.get(f'http://{pico_id}/dwc')
    return f'Sent command to Pico, Response: {response.text}', 200

def off_robot():
    print("OFF")
    response = requests.get(f'http://{pico_id}/stop')
    return f'Sent command to Pico, Response: {response.text}', 200

@app.route('/pico_control', methods=['GET'])
def pico_control():
    dry = request.args.get('dry')
    wet = request.args.get('wet')
    dry_wet = request.args.get('dry_wet')
    if dry == "true":
        dry_clean()
    elif wet == "true":
        wet_clean()
    elif dry_wet == "true":
        dry_wet_clean()
    else:
        off_robot()
    

@app.route('/alarm_control', methods=['GET'])
def alarm_control():
    hour = request.args.get('HH')
    minute = request.args.get('MM')
    if int(hour) < 10 :
        shedule_time = f"0{hour}"
    else:
        shedule_time = f"{hour}"
    
    shedule_time += ":"

    if int(minute) < 10 :
        shedule_time += f"0{minute}"
    else:
        shedule_time += f"{minute}"

    # shedule the cleaning 
    schedule.every().day.at(shedule_time).do(dry_clean)
    return   200

# Define a function to run the scheduler in a separate thread
def schedule_thread():
    while True:
        schedule.run_pending()
        time.sleep(1)

# Start the scheduling thread
thread = threading.Thread(target=schedule_thread)
thread.start()


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
