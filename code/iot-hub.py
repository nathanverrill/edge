import board
import time
from adafruit_bme280 import basic as adafruit_bme280
import os
import asyncio
from azure.iot.device import IoTHubDeviceClient, Message
import json
from datetime import timezone
import datetime
import psutil

async def main():

    # Create sensor object, using the board's default I2C bus.
    i2c = board.I2C()   # uses board.SCL and board.SDA
    bme280 = adafruit_bme280.Adafruit_BME280_I2C(i2c)

    # change this to match the location's pressure (hPa) at sea level
    bme280.sea_level_pressure = 1013.25

    # Fetch the connection string from an environment variable
    conn = os.getenv("IOTHUB_DEVICE_CONNECTION_STRING")
    print(conn)

    # Create instance of the device client using the authentication provider
    device_client = IoTHubDeviceClient.create_from_connection_string(conn)
    # Connect the device client.
    device_client.connect()

    while True:
        # Get cpu statistics
        cpu = str(psutil.cpu_percent()) + '%'
        # Calculate memory information
        memory = psutil.virtual_memory()
        # Convert Bytes to MB (Bytes -> KB -> MB)
        available = round(memory.available/1024.0/1024.0,1)
        total = round(memory.total/1024.0/1024.0,1)
        mem_info = str(available) + 'MB free / ' + str(total) + 'MB total ( ' + str(memory.percent) + '% )'
        # Calculate disk information
        disk = psutil.disk_usage('/')
        # Convert Bytes to GB (Bytes -> KB -> MB -> GB)
        free = round(disk.free/1024.0/1024.0/1024.0,1)
        total = round(disk.total/1024.0/1024.0/1024.0,1)
        disk_info = str(free) + 'GB free / ' + str(total) + 'GB total ( ' + str(disk.percent) + '% )'
        # Send a single message
        print("Sending message...")
        msg = '{{"environment": "dev", "messageType": "system-status", "deviceId": "rp4b", "cpu": "{cp}", "memory": "{mem}", "disk_info": "{disk}"}}'
        msg = msg.format(cp=cpu, mem=mem_info, disk=disk_info)
        messagesys = Message(msg)
        messagesys.content_encoding = "utf-8"
        messagesys.content_type = "application/json"
        #msgJSON = json.dumps(msg)
        device_client.send_message(messagesys)
        print(messagesys)
        time.sleep(10)
        # Send a single message
        print("Sending message...")
        msg = '{{"environment": "dev", "messageType": "environment-status", "deviceId": "rp4b", "temperature": {temp}, "humidity": {humid}, "pressure": {press}, "altitude": {alt}}}'
        #msgJSON = json.dumps(msg)
        msg = msg.format(temp=bme280.temperature, humid=bme280.relative_humidity, press=bme280.pressure, alt=bme280.altitude)
        messageenv = Message(msg)
        messageenv.content_encoding = "utf-8"
        messageenv.content_type = "application/json"
        device_client.send_message(messageenv)
        print(messageenv)
        
        # test message
        metrics = {}
        metrics['environment'] = 'dev'
        metrics['messageType'] = 'metrics-test'
        metrics['deviceId'] = 'rp4b'
        metrics['accX'] = 0.987
        metrics['accY'] = 0.123
        metrics['accZ'] = 0.314
        
        iothub_metrics = Message(json.dumps(metrics))
        iothub_metrics.content_encoding = "utf-8"
        iothub_metrics.content_type = "application/json"
        device_client.send_message(iothub_metrics)      
        print(json.dumps(metrics))  
        
        
        
        time.sleep(5)

    # finally, shut down the client
    device_client.shutdown()

if __name__ == "__main__":
    asyncio.run(main())