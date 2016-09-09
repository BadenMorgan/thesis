import time
import RPi.GPIO as GPIO
import serial
import binascii
from array import array
import numpy as np
# import MySQLdb
import os
import pymssql
import _mssql
import decimal
import uuid
import httplib2
import urllib.request
import pymysql
import sys
from select import select

GPIO.setmode(GPIO.BCM)

# UART Setup
ser1 = serial.Serial(
              
               port='/dev/ttyAMA0',
               baudrate = 9600,
               parity=serial.PARITY_NONE,
               stopbits=serial.STOPBITS_ONE,
               bytesize=serial.EIGHTBITS,
               timeout=1
           )

# ser = serial.Serial(
# 	port = '/dev/ttyACM0',
# 	baudrate=9600,
# 	parity=serial.PARITY_ODD,
# 	stopbits=serial.STOPBITS_TWO,
# 	bytesize=serial.SEVENBITS
# 	)

# initializers
# pin setup
DE = 23
RE = 24
door1 = 20
door2 = 21

GPIO.setwarnings(False)
GPIO.setup(DE, GPIO.OUT)
GPIO.setup(RE, GPIO.OUT)
GPIO.setup(door1, GPIO.OUT)
GPIO.setup(door2, GPIO.IN)
GPIO.output(RE, GPIO.LOW)
GPIO.output(DE, GPIO.LOW)
GPIO.output(door1, GPIO.HIGH)


#sudo macros/varaibles
TXBuffer = bytearray([0xA1, 0x81, 0xB3, 0x01, 214, 0xF1])
checksum = 0
FlushDate = time.strftime("%Y-%m-%d")
command = ''
timeout = 9000;#seconds
currentTimeout = timeout
exitFlag = 0
tagLength = 14
value = 0
retries = 0
maxretry = 5;
DispenseStatus = 0;

# admins for the VM
admins = ('MRGBAD001','01422682','Brendan')

# function definitions
# checsum
def checksumcal():
	check = TXBuffer[0]
	check = (check + TXBuffer[1]) & 0xFF
	check = (check + TXBuffer[2]) & 0xFF
	check = (check + TXBuffer[3]) & 0xFF 			
	return check

# funtion the will send user freindly message to LCD on
# vending machine in order to give an update on how to dispnce went
def FinalMsg():
	global DispenseStatus

	if DispenseStatus == 1:
		UpdateLCD("There was a problem delivering one of your components, sorry for the inconvenience")
	elif DispenseStatus == 2:
		UpdateLCD("One of the components you ordered is out of stock, sorry for the inconvenience")
	elif DispenseStatus == 3:
		UpdateLCD("There was a problem with delivery and one of the components you ordered is out of stock, sorry for the inconvenience")
	else:
		UpdateLCD("Thank You for using UCT's Vending Machine")

	DispenseStatus = 0

# function to request a dispence
def DispenceIC(addressbyte,commandbyte,valuebyte):
	global DispenseStatus

	if valuebyte != 0 and GPIO.input(door2) == 0:
		print("Cant dispence door is open")
		UpdateLCD("Cant dispence   gdoor is open")
	else:
		global retries
		global maxretry
		if valuebyte != 0:
			UpdateLCD("Dispensing      Components")
		TXBuffer[1] = addressbyte
		TXBuffer[2] = commandbyte
		TXBuffer[3] = valuebyte
		TXBuffer[4] = checksumcal()

		GPIO.output(RE, GPIO.HIGH)
		GPIO.output(DE, GPIO.HIGH)
		time.sleep(0.1)
		ser1.write(TXBuffer)
		time.sleep(0.1)
		GPIO.output(DE, GPIO.LOW)
		GPIO.output(RE, GPIO.LOW)

		time.sleep(1.9*valuebyte)
		value = {}
		if(ser1.inWaiting() > 0):
			value = ser1.read(6)
			# print("Reading response:")
			# print(value[2])

			while(ser1.inWaiting() > 0):
				print("Flushing extra reads...")
				ser1.read(ser1.inWaiting()) #flushing the system.
				print("Done!")

		elif (ser1.inWaiting() > 6):
			print("Too much data in buffer - flushing")
			time.sleep(2)
			print(ser1.inWaiting())
			print(ser1.read(ser1.inWaiting())) #flushing the system.
			print("Flushed")
		else:
			time.sleep(1.4*valuebyte)
			if(ser1.inWaiting() > 0):
				value = ser1.read(6)
				print("Reading response:")
				print(value[2])
				

				while(ser1.inWaiting() > 0):
					print("Flushing extra reads...")
					ser1.read(ser1.inWaiting()) #flushing the system.
					print("Done!")

			elif (ser1.inWaiting() > 6):
				print("Too much data in buffer - flushing")
				time.sleep(2)
				print(ser1.inWaiting())
				print(ser1.read(ser1.inWaiting())) #flushing the system.
				print("Flushed")
			else:
				print("Failed Dispense")
		check = value[0]
		check = (check + value[1]) & 0xFF
		check = (check + value[2]) & 0xFF
		check = (check + value[3]) & 0xFF
		if value[0] == 0xD1 and value[1] == addressbyte and value[3] == 1 and value[4] == check and value[5] == 0xE1:
			retries = 0
			if value[2] & 0x01:
				print("Jammed")
				DispenseStatus  = DispenseStatus | 1
				UpdateJEL(0,addressbyte)
			if value[2] & 0x04:
				print("Low")
				UpdateJEL(2,addressbyte)
			if value[2] & 0x08:
				print("Dispensed Successfully")
				if value[2] & 0x02:
					print("Empty")
					UpdateJEL(1,addressbyte)
			elif value[2] & 0x02:
				print("Empty")
				DispenseStatus  = DispenseStatus | 2
				UpdateJEL(1,addressbyte)

		else:
			print("Failed")
	return 

# function to LCD UPDATE
def UpdateLCD(StringToPrint):
	global retries
	global maxretry

	LCDBuffer = bytearray([0xA1,0x01,0xB7])
	LCDBuffer.extend(bytearray(map(ord,StringToPrint)))
	LCDBuffer.extend([0x04,0xF1])
	
	GPIO.output(RE, GPIO.HIGH)
	GPIO.output(DE, GPIO.HIGH)
	time.sleep(0.1)
	ser1.write(LCDBuffer)
	time.sleep(0.1)
	GPIO.output(DE, GPIO.LOW)
	GPIO.output(RE, GPIO.LOW)

	time.sleep(1.2)
	value = {}
	if(ser1.inWaiting() > 0):
		value = ser1.read(6)
		# print("Reading response:")
		# print(value[2])
		

		while(ser1.inWaiting() > 0):
			print("Flushing extra reads...")
			ser1.read(ser1.inWaiting()) #flushing the system.
			print("Done!")
	
		check = value[0]
		check = (check + value[1]) & 0xFF
		check = (check + value[2]) & 0xFF
		check = (check + value[3]) & 0xFF
		if value[0] == 0xD1 and value[1] == 1 and value[3] == 1 and value[4] == check and value[5] == 0xE1:
			if value[2] & 0x08:
				print("Displayed Successfully")
				retries = 0
		else:
			print("display failed Failed")
			if retries < maxretry:
				retries = retries + 1
				UpdateLCD(StringToPrint)

	elif (ser1.inWaiting() > 6):
		print("Too much data in buffer - flushing")
		time.sleep(2)
		print(ser1.inWaiting())
		print(ser1.read(ser1.inWaiting())) #flushing the system.
		print("Flushed")

	else:
		print("retrying")
		if retries < maxretry:
			retries = retries + 1
			UpdateLCD(StringToPrint)
		else:
			retries = 0

	return 

# request dispencary on all that match the student number
# and if the entry has not been used
def RequestDispence(studentNo):
	db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
	cursor=db.cursor()
	cursor.execute("""SELECT PartName from Orders WHERE StudentNo = %s""", (studentNo))
	Part = cursor.fetchall()
	cursor.execute("""SELECT Quantity from Orders WHERE StudentNo = %s""", (studentNo))
	Quantity = cursor.fetchall()
	cursor.execute("""SELECT Done from Orders WHERE StudentNo = %s""", (studentNo))
	Done = cursor.fetchall()
	i = 0
	PartLen = len(Part)
	while i < PartLen:
		if Done[i][0] == 0  and Quantity[i][0] > 0:
			print("go")
			rowlen = cursor.execute("""SELECT address from Components WHERE PartName = %s""", (Part[i][0]))
			row = cursor.fetchone()
			if(rowlen > 0):
				DispenceIC(row[0],0xB3,Quantity[i][0])
				time.sleep(8)
		i = i + 1
	cursor.execute("""UPDATE Orders SET Done = 1 WHERE StudentNo = %s""", (studentNo))
	db.commit()
	cursor.close()
	db.close()
	print("done")

# flsuhes the database at midnight
def FlushDB():
	global FlushDate
	currentdate = time.strftime("%Y-%m-%d")
	if currentdate != FlushDate:
		db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
		cursor=db.cursor()
		cursor.execute("""DELETE FROM Orders WHERE Date = %s""", (FlushDate))
		db.commit()
		cursor.close()
		db.close()
		FlushDate = currentdate
		print("flushed")

# free up the jam empty status
def Free():

	db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
	cursor=db.cursor()
	rowlen = cursor.execute("""SELECT address from Components WHERE Jam = 1""")
	if(rowlen > 0):
		row = cursor.fetchall()
		i = 0
		for i in range (0, rowlen):	
			DispenceIC(row[i][0],0xB5,0)
			cursor.execute("""UPDATE Components SET Jam = 0 WHERE Address = %s""", (row[i]))
			time.sleep(1)
			i = i + 1

	rowlen = cursor.execute("""SELECT address from Components WHERE Empty = 1""")
	if(rowlen > 0):
		row = cursor.fetchall()
		i = 0
		for i in range (0, rowlen):	
			cursor.execute("""UPDATE Components SET Empty = 0 WHERE Address = %s""", (row[i]))
			time.sleep(1)
			i = i + 1

	rowlen = cursor.execute("""SELECT address from Components WHERE Low = 1""")
	if(rowlen > 0):
		row = cursor.fetchall()
		i = 0
		for i in range (0, rowlen):	
			cursor.execute("""UPDATE Components SET Low = 0 WHERE Address = %s""", (row[i]))
			time.sleep(1)
			i = i + 1

	db.commit()
	cursor.close()
	db.close()
	UpdateLCD("Freed up all    modules")
	print('free')

# update database for jam/empty/low
def UpdateJEL(JEL, address):
	db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
	cursor=db.cursor()

	if JEL == 0:
		cursor.execute("""UPDATE Components SET Jam = 1 WHERE Address = %s""", address)
	elif JEL == 1:
		cursor.execute("""UPDATE Components SET Empty = 1 WHERE Address = %s""", address)
	elif JEL == 2:
		cursor.execute("""UPDATE Components SET Low = 1 WHERE Address = %s""", address)
			
	db.commit()
	cursor.close()
	db.close()	

# connect to uct db and reqeust student number
# def RequestStNo(ID):
# 	print("searching db")
# 	conn = pymssql.connect('srvwinsqlvs007.wf.uct.ac.za\DW', 'uctaccessreports', 'uct@cc3$$r3p0rt$', 'DB400_reports')  #Username and password missing
# 	cursor = conn.cursor()
# 	sqlquery = """EXEC DB400_reports.dbo.GET_TAG_DETAILS @TAG_CODE = %s""" % ID
# 	cursor.execute(sqlquery)
# 	info = cursor.fetchone()	
# 	if(len(info) > 0):
# 		i = 0
# 		for i in range (0, len(admins)):
# 			if info[0] == admins[i]:
# 				print('freeing up ics')
# 				Free();	
# 				command = ''
# 			i = i + 1
# 		print('requesting')		
# 		return RequestDispence(info[0])
# 	else:
# 		print('found nothing')
# 	ser.close()
# 	return

# msg = bytearray([0x48,0x45,0x4C,0x4C,0x4F,0x0A])
# ser1.write('hello\n')
print('hello')

# try:
while True:
	# try:
	# if(ser.inWaiting()>0):
	# 	currentTimeout = timeout;
	# if(ser.inWaiting()%tagLength == 0):
	# 	value = ser.read(tagLength)
	# 	print("Reading card...")
	# 	value = value.decode("utf-8")
	# 	value = int(value[1:-3],16)
	# 	print ("Please remove card!")

	# 	while(ser.inWaiting() > 0):
	# 		print("Flushing extra reads...")
	# 		ser.read(ser.inWaiting()) #flushing the system.
	# 		print("Done!")
		
	# 	RequestStNo(value)			
	# elif (ser.inWaiting() > tagLength):
	# 	print("Too much data in buffer - flushing")
	# 	time.sleep(2)
	# 	print(ser.inWaiting())
	# 	print(ser.read(ser.inWaiting())) #flushing the system.
	# 	print("Flushed")

	# else:
	# 	time.sleep(1)
	# 	currentTimeout-=1;	
	# except KeyboardInterrupt:
	# 	print("keyboard out!")
	# 	sys.exit()
	# except:
	# 	print('ERROR')





	# FlushDB()
	# rlist, _, _ = select([sys.stdin], [], [], 1)
	# if rlist:		
	# 	command = sys.stdin.readline()
	# 	sys.stdin.flush()	
	command = input("enter g: ")
	if command == 'g':
		# RequestDispence('MRGBAD001')
		# RequestStNo('81607133871')
		# print("despensing")
		DispenceIC(1,0xB3,3)
		FinalMsg()
		# 
		command = ''
	if command == 'f':
		Free();
		# i = 0
		# for i in range (0, len(admins)):
		# 	print(admins[i])
		# 	if 'MRGBAD001' == admins[i]:
					
		# 		command = ''
		# 	i = i + 1

# except KeyboardInterrupt:
# 	print("keyboard out!")
# except:
# 	print ("what")
# finally:
# 	GPIO.cleanup()