
#Note : Give a delay in the arduino code of atleast 20ms
import serial
while 1:
	with serial.Serial('/dev/ttyUSB0',9600) as ser:
		ser.flushInput()	
		line = ser.readline()
		
		with open('result.txt','a') as myfile:
			myfile.write(line)




