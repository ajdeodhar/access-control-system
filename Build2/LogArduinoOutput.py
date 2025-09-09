import serial
import time

# Set your COM port and baud rate
ser = serial.Serial('COM8', 9600)  # Change COM3 to your Arduino port

log_file_path = r"D:\Anshuman\Anshuman\UMass\Summer 2025\JDP\Build2\Sketch_Code2_Logs.txt"

with open(log_file_path, 'a') as f:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line:
            print(line)  # Show in terminal
            timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
            f.write(f"{timestamp} -> {line}\n")
            f.flush()
