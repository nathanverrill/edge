# nano_33_ble_sense_serial_out.py
# Nathan Verrill, Jan 2023, SAIC, nathan.verrill@saic.com
# read serial port, format and write data to mqtt

########################################################################
# Due to issue updating code on Arduino 33 BLE Sense, the metadata output columns do not match the sensor data
# so there is some placeholder code to handle.
########################################################################

from os import system           # Interact with the operating system to get ports
import serial                   # https://pyserial.readthedocs.io/en/latest/index.html
from time import sleep

# get port list
portList = system('python3 -m serial.tools.list_ports ')
print(portList)

# use port scanning
serialPort = "/dev/cu.usbmodem21101"

# Optimized serial port reading
# https://github.com/pyserial/pyserial/issues/216
class ReadLine:
    def __init__(self, s):
        self.buf = bytearray()
        self.s = s
    
    def readline(self):
        i = self.buf.find(b"\n")
        if i >= 0:
            r = self.buf[:i+1]
            self.buf = self.buf[i+1:]
            return r
        while True:
            i = max(1, min(2048, self.s.in_waiting))
            data = self.s.read(i)
            i = data.find(b"\n")
            if i >= 0:
                r = self.buf + data[:i+1]
                self.buf[0:] = data[i+1:]
                return r
            else:
                self.buf.extend(data)


# continous read
with serial.Serial(serialPort, 19200, timeout=1) as ser:
    
    reader = ReadLine(ser)
    
    while True:
        
        print(reader.readline())
        
        ser.flushOutput()               # prevent device read from filling up and hanging