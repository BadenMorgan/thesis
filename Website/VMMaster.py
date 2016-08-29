import time
import RPi.GPIO as GPIO
import serial
import binascii
import MySQLdb
import os

# mysql connection
db = MySQLdb.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
cursor=db.cursor()

GPIO.setmode(GPIO.BCM)

# UART Setup
ser = serial.Serial(
              
               port='/dev/serial0',
               baudrate = 9600,
               parity=serial.PARITY_NONE,
               stopbits=serial.STOPBITS_ONE,
               bytesize=serial.EIGHTBITS,
               timeout=1
           )

# initializers
# pin setup
# GPIO.setup(23, GPIO.OUT)
# GPIO.setup(24, GPIO.OUT)
# GPIO.setup(20, GPIO.OUT)
# GPIO.setup(21, GPIO.IN)

#sudo macros/varaibles
TXBuffer = bytearray([0xA1, 0x81, 0xB3, 0x01, 214, 0xF1])
checksum = 0;

# function definitions
# checsum
def checksumcal():
	check = TXBuffer[0]
	check = (check + TXBuffer[1]) & 0xFF
	check = (check + TXBuffer[2]) & 0xFF
	check = (check + TXBuffer[3]) & 0xFF 			
	return check

# function to request a dispence
def DispenceIC(addressbyte,commandbyte,valuebyte):
	TXBuffer[1] = addressbyte
	TXBuffer[2] = commandbyte
	TXBuffer[3] = valuebyte
	TXBuffer[4] = checksumcal()
	ser.write(TXBuffer)
	return 



# msg = bytearray([0x48,0x45,0x4C,0x4C,0x4F,0x0A])
# ser.write('hello\n')
print('hello')

# try:
while True:
	print "enter command"
	command = raw_input()
	if (command == 'g'):
		cursor.execute("""SELECT address from Components WHERE PartName = %s""", ('555'))
		row = cursor.fetchone()
		print row[0]
		val = int(raw_input())
		DispenceIC(row[0],0xB3,val)			
# except KeyboardInterrupt:
	# print "keyboard out!"
# except:
	# print "what"
	# os.system("VMMaster.py 1")
# finally:
	# GPIO.cleanup()
	# db.close()


