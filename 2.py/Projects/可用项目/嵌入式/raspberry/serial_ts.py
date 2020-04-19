import serial
ser = serial.Serial('/dev/ttyUSB0',9600,timeout=1)
ser.write('t'.encode())
response = ser.readall()
print(response)
