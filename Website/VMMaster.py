import time
import RPi.GPIO as GPIO
import serial
import binascii

GPIO.setmode(GPIO.BCM)

# UART Setup
ser = serial.Serial(
              
               port='/dev/serial0',
               baudrate = 115200,
               parity=serial.PARITY_NONE,
               stopbits=serial.STOPBITS_ONE,
               bytesize=serial.EIGHTBITS,
               timeout=1
           )

# initializers
# pin setup
GPIO.setup(23, GPIO.OUT)
GPIO.setup(24, GPIO.OUT)
GPIO.setup(20, GPIO.OUT)
GPIO.setup(21, GPIO.IN)

#sudo macros/varaibles
TXBuffer = bytearray([0xA1, 0x81, 0xB3, 0x01, 214, 0xF1])
checksum = 0;
# function definitions
# CRC-8 - based on the CRC8 formulas by Dallas/Maxim
# code released under the therms of the GNU GPL 3.0 license
def checksumcal():
	check = TXBuffer[0]
	check = (check + TXBuffer[1]) & 0xFF
	check = (check + TXBuffer[2]) & 0xFF
	check = (check + TXBuffer[3]) & 0xFF 			
	return check



# msg = bytearray([0x48,0x45,0x4C,0x4C,0x4F,0x0A])
# ser.write('hello\n')
print('hello')

try:
	while True:
		print "enter command"
		command = raw_input()
		if (command == 'g'):
			quantity = raw_input()
			quantity = int(quantity)
			print "quantity: "
			print quantity
			TXBuffer[3] = quantity
			print("checksum: ")
			checksum = checksumcal()
			print checksum			
			TXBuffer[4] = checksum
			ser.write(TXBuffer)
			
except KeyboardInterrupt:
	print "keyboard out!"
except:
	print "what"

finally:
	GPIO.cleanup()

