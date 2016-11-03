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
import logging

LOG_FILENAME = 'MasterLog.log'

# Set up a specific logger with our desired output level
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s %(name)-12s %(levelname)-8s %(message)s',
                    datefmt='%m-%d %H:%M',
                    filename='/var/www/html/MasterLog.log')
                    # ,
                    # filemode='w')

logging.debug("=========================")
logging.debug("Program Started")

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

ser = serial.Serial(
	port = '/dev/ttyACM0',
	baudrate=9600,
	parity=serial.PARITY_ODD,
	stopbits=serial.STOPBITS_TWO,
	bytesize=serial.SEVENBITS
	)

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
maxretry = 0
DispenseStatus = 0
# component limit
complimit = 6

# admins for the VM
admins = ('MRGBAD001','01422682','Brendan')

# function definitions
# checsum
def checksumcal():
	try:
		check = TXBuffer[0]
		check = (check + TXBuffer[1]) & 0xFF
		check = (check + TXBuffer[2]) & 0xFF
		check = (check + TXBuffer[3]) & 0xFF 			
		return check
	except:
		logging.warning("checksum error?")

# funtion the will send user freindly message to LCD on
# vending machine in order to give an update on how to dispnce went
def FinalMsg():
	try:
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
	except:
		logging.warning("final message error?")

# function to request a dispence
def DispenceIC(addressbyte,commandbyte,valuebyte):
	try:
		global DispenseStatus

		if valuebyte != 0 and GPIO.input(door2) == 0:
			logging.debug("Cant dispence door is open")
			UpdateLCD("Cant dispence   gdoor is open")
		else:
			global retries
			global maxretry
			# if valuebyte != 0:
				# UpdateLCD("Dispensing      Components")
			TXBuffer[1] = addressbyte
			TXBuffer[2] = commandbyte
			TXBuffer[3] = valuebyte
			TXBuffer[4] = checksumcal()

			GPIO.output(DE, GPIO.HIGH)
			time.sleep(0.1)
			ser1.write(TXBuffer)
			time.sleep(0.1)
			GPIO.output(DE, GPIO.LOW)

			time.sleep(1.9*valuebyte)
			value2 = {}
			value = {}
			if(ser1.inWaiting() > 0):
				value2 = ser1.read(6)
				value = ser1.read(6)
				print(value2)
				print(value)
				# logging.debug("Reading response:")
				# logging.debug(value[2])

				while(ser1.inWaiting() > 0):
					logging.debug("Flushing extra reads...")
					ser1.read(ser1.inWaiting()) #flushing the system.
					logging.debug("Done!")

			elif (ser1.inWaiting() > 6):
				logging.debug("Too much data in buffer - flushing")
				time.sleep(2)
				logging.debug(ser1.inWaiting())
				logging.debug(ser1.read(ser1.inWaiting())) #flushing the system.
				logging.debug("Flushed")
			else:
				time.sleep(1.4*valuebyte)
				if(ser1.inWaiting() > 0):
					value = ser1.read(6)
					logging.debug("Reading response:")
					logging.debug(value[2])
					

					while(ser1.inWaiting() > 0):
						logging.debug("Flushing extra reads...")
						ser1.read(ser1.inWaiting()) #flushing the system.
						logging.debug("Done!")

				elif (ser1.inWaiting() > 6):
					logging.debug("Too much data in buffer - flushing")
					time.sleep(2)
					logging.debug(ser1.inWaiting())
					logging.debug(ser1.read(ser1.inWaiting())) #flushing the system.
					logging.debug("Flushed")
				else:
					logging.debug("Failed Dispense")
			try:
				check = value[0]
				# print(check)
				check = (check + value[1]) & 0xFF
				# print(check)
				check = (check + value[2]) & 0xFF
				# print(check)
				check = (check + value[3]) & 0xFF
				# print(check)
				if value[0] == 0xD1 and value[1] == addressbyte and value[3] == 1 and value[4] == check and value[5] == 0xE1:
					retries = 0
					if value[2] & 0x01:
						logging.debug("Jammed")
						DispenseStatus  = DispenseStatus | 1
						UpdateJEL(0,addressbyte)
					if value[2] & 0x04:
						logging.debug("Low")
						UpdateJEL(2,addressbyte)
					if value[2] & 0x08:
						logging.debug("Dispensed Successfully")
						if value[2] & 0x02:
							logging.debug("Empty")
							UpdateJEL(1,addressbyte)
					elif value[2] & 0x02:
						logging.debug("Empty")
						DispenseStatus  = DispenseStatus | 2
						UpdateJEL(1,addressbyte)

				else:
					logging.debug("Failed")
			except:
				logging.warning("Communications failure, Dispense")
		return 
	except:
		logging.warning("failed to dispense a component")

# function to LCD UPDATE
def UpdateLCD(StringToPrint):
	# try:
	global retries
	global maxretry

	discard = len(StringToPrint) + 5

	LCDBuffer = bytearray([0xA1,0x01,0xB7])
	LCDBuffer.extend(bytearray(map(ord,StringToPrint)))
	LCDBuffer.extend([0x04,0xF1])
	
	# GPIO.output(RE, GPIO.HIGH)
	GPIO.output(DE, GPIO.HIGH)
	time.sleep(0.1)
	ser1.write(LCDBuffer)
	time.sleep(0.1)
	GPIO.output(DE, GPIO.LOW)
	# GPIO.output(RE, GPIO.LOW)

	time.sleep(3)
	value = {}
	value2 = {}
	if(ser1.inWaiting() > 0):
		value2 = ser1.read(discard)
		value = ser1.read(6)
		# logging.debug("Reading response:")
		# logging.debug(value[2])
		print(value2)
		print(value)

		while(ser1.inWaiting() > 0):
			logging.debug("Flushing extra reads...")
			ser1.read(ser1.inWaiting()) #flushing the system.
			logging.debug("Done!")
		try:
			# gprint("checking")
			check = value[0]
			# print(check)
			check = (check + value[1]) & 0xFF
			# print(check)
			check = (check + value[2]) & 0xFF
			# print(check)
			check = (check + value[3]) & 0xFF
			# print(check)
			# print('wtf')
			# print(value[0])
			# print(value[1])
			# print(value[2])
			# print(value[3])
			# print(value[4])
			# print(value[5])
			if value[0] == 0xD1 and value[1] == 1 and value[3] == 1 and value[4] == check and value[5] == 0xE1:
				if value[2] & 0x08:
					logging.debug("Displayed Successfully")
					retries = 0
			else:
				logging.debug("display failed Failed")
				if retries < maxretry:
					retries = retries + 1
					UpdateLCD(StringToPrint)
				else:
					retries = 0
		except:
			logging.warning("Communications failure, LCD")

	elif (ser1.inWaiting() > 6):
		logging.debug("Too much data in buffer - flushing")
		time.sleep(2)
		logging.debug(ser1.inWaiting())
		logging.debug(ser1.read(ser1.inWaiting())) #flushing the system.
		logging.debug("Flushed")

	else:
		logging.debug("retrying LCD")
		if retries < maxretry:
			retries = retries + 1
			UpdateLCD(StringToPrint)
		else:
			retries = 0

	return 
	# except:
	# 	logging.warning("failed to update the LCD")

# request dispencary on all that match the student number
# and if the entry has not been used
def RequestDispence(studentNo):
	try:
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
				logging.debug("go")
				rowlen = cursor.execute("""SELECT address from Components WHERE PartName = %s""", (Part[i][0]))
				row = cursor.fetchone()
				if(rowlen > 0):
					DispenceIC(row[0],0xB3,Quantity[i][0])
					# time.sleep(8)
			i = i + 1
		cursor.execute("""UPDATE Orders SET Done = 1 WHERE StudentNo = %s""", (studentNo))
		db.commit()
		cursor.close()
		db.close()
		logging.debug("Finished Dispensing")
		FinalMsg()
	except:
		logging.warning("failed to dispense with student number")

# flsuhes the database at midnight
def FlushDB():
	try:
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
			logging.debug("flushed")
	except:
		logging.warning("failed to flush database")

# makes sure there are only 6 components for each student no
def ComponentNoCheck(ID):
	try:
		logmsg = "Checking Order Numbers for: " + ID
		global complimit
		db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
		cursor=db.cursor()

		cursor.execute("""SELECT Quantity,ID from Orders WHERE StudentNo = %s""", (ID))
		Order = cursor.fetchall()
		OrderLen = len(Order)
		i = 0
		count = 0
		while i < OrderLen:
			if count < complimit:
				tempcount = count + Order[i][0]
				if tempcount > complimit:
					quantity = complimit - count
					count = complimit
					cursor.execute("""UPDATE Orders SET Quantity = %s WHERE ID = %s""", (quantity,Order[i][1]))
				else:
					count = tempcount
			else:
				cursor.execute("""DELETE FROM Orders WHERE ID = %s""", (Order[i][1]))
			i = i + 1
		logmsg = "Checked Order Numbers for: " + ID
		logging.debug(logmsg)
		db.commit()
		cursor.close()
		db.close()
	except:
		logging.warning("Failed to check Quantity numbers")


# free up the jam empty status
def Free():
	try:
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
		logging.debug('free')
	except:
		logging.warning("failed to free up modules")

# update database for jam/empty/low
def UpdateJEL(JEL, address):
	try:
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
	except:
		logging.warning("failed to Update database on jam, empty or low status")

def mailadmin(address, report):
	fo = open("email.php", "w+")
	fo.write('<?php\n')
	fo.write("include 'includes/variables.php';\n")
	fo.write('$fromHeader = "From: Vending Machie Report";\n')
	reportcode = ''
	if report == 0:
		reportcode = "JAM"
	else:
		reportcode = "EMPTY"
	fo.write('$text = "The Vending Machine has a:\\n' + reportcode + '\\nThe device reporting is at address, shown as integer (binary):\\n' + str(address) + ' (' + str('{0:08b}'.format(address)) + ')\\nPlease attend to it as soon as possible.";\n')
	fo.write('mail($EmailAdmin,"Vending Machine Report" , "From: Vending Machine\\n\\n".$text."\\n\\n"."Sent: ".date("h:i:sa d-m-Y"),$fromHeader);\n')
	fo.write('?>\n')
	fo.close()
	os.system('sudo mv email.php /var/www/html/')
	os.system('sudo php -f /var/www/html/email.php')
	os.system('sudo rm /var/www/html/email.php')
	return
	

def callModules():
	# try:
	global retries
	global maxretry
	db = pymysql.connect("localhost", "root", "pimysql2016", "UCTVendingMachine")
	cursor=db.cursor()
	cursor.execute("""SELECT Address from Components""")
	Address = cursor.fetchall()
	i = 0
	LenAddress = len(Address)
	while i < LenAddress:
		TXBuffer[1] = Address[i][0]
		TXBuffer[2] = 0xB1
		TXBuffer[3] = 0
		TXBuffer[4] = checksumcal()


		GPIO.output(DE, GPIO.HIGH)
		time.sleep(0.1)
		ser1.write(TXBuffer)
		time.sleep(0.1)
		GPIO.output(DE, GPIO.LOW)

		time.sleep(0.5)
		
		value = {}
		if(ser1.inWaiting() > 0):
			value = ser1.read(6)
			# logging.debug("Reading response:")
			# logging.debug(value[2])
			

			while(ser1.inWaiting() > 0):
				logging.debug("Flushing extra reads...")
				ser1.read(ser1.inWaiting()) #flushing the system.
				logging.debug("Done!")
			try:
				check = value[0]
				check = (check + value[1]) & 0xFF
				check = (check + value[2]) & 0xFF
				check = (check + value[3]) & 0xFF
				if value[0] == 0xD1 and value[1] == Address[i] and value[3] == 1 and value[4] == check and value[5] == 0xE1:
					if value[2] == 0xC8:
						logging.debug("Successfuly called " + Address[0])
						retries = 0
						i = i + 1
				else:
					logging.debug("failed to call")
					if retries < maxretry:
						retries = retries + 1
					else:
						cursor.execute("""UPDATE Components SET Empty = 1 WHERE Address = %s""", Address[i][0])
						retries = 0
						i = i + 1
			except:
				logging.warning("Communications failure, calling")

		elif (ser1.inWaiting() > 6):
			logging.debug("Too much data in buffer - flushing")
			time.sleep(2)
			logging.debug(ser1.inWaiting())
			logging.debug(ser1.read(ser1.inWaiting())) #flushing the system.
			logging.debug("Flushed")

		else:
			logging.debug("retrying call")
			if retries < maxretry:
				retries = retries + 1
			else:
				retries = 0
				cursor.execute("""UPDATE Components SET Empty = 1 WHERE Address = %s""", Address[i][0])
				i = i + 1

		

	db.commit()
	cursor.close()
	db.close()
	logging.debug("Finished calling")
	# except:
	# 	logging.warning("failed to call modules one by one")

# connect to uct db and reqeust student number
def RequestStNo(ID):
	logging.debug("searching db")
	conn = pymssql.connect('srvwinsqlvs007.wf.uct.ac.za\DW', 'uctaccessreports', 'uct@cc3$$r3p0rt$', 'DB400_reports')  #Username and password missing
	cursor = conn.cursor()
	sqlquery = """EXEC DB400_reports.dbo.GET_TAG_DETAILS @TAG_CODE = %s""" % ID
	cursor.execute(sqlquery)
	info = cursor.fetchone()	
	if(len(info) > 0):
		i = 0
		for i in range (0, len(admins)):
			if info[0] == admins[i]:
				logging.debug('freeing up ics')
				Free();	
				command = ''
			i = i + 1
		logging.debug('requesting')		
		return RequestDispence(info[0])
	else:
		logging.debug('found nothing')
	ser.close()
	return

# msg = bytearray([0x48,0x45,0x4C,0x4C,0x4F,0x0A])
# ser1.write('hello\n')

# try:
# time.sleep(5)
# callModules()
while True:
	# try:
	if(ser.inWaiting()>0):
		currentTimeout = timeout;
	if(ser.inWaiting()%tagLength == 0):
		value = ser.read(tagLength)
		logging.debug("Reading card...")
		value = value.decode("utf-8")
		value = int(value[1:-3],16)

		while(ser.inWaiting() > 0):
			logging.debug("Flushing extra reads...")
			ser.read(ser.inWaiting()) #flushing the system.
			logging.debug("Done!")
		
		RequestStNo(value)			
	elif (ser.inWaiting() > tagLength):
		logging.debug("Too much data in buffer - flushing")
		time.sleep(2)
		logging.debug(ser.inWaiting())
		logging.debug(ser.read(ser.inWaiting())) #flushing the system.
		logging.debug("Flushed")

	else:
		time.sleep(1)
		currentTimeout-=1;	
		# except KeyboardInterrupt:
		# 	logging.debug("keyboard out!")
		# 	sys.exit()
		# except:
		# 	logging.debug('ERROR')





		# FlushDB()
		# rlist, _, _ = select([sys.stdin], [], [], 1)
		# if rlist:		
		# 	command = sys.stdin.readline()
		# 	sys.stdin.flush()

		# ComponentNoCheck("MRGBAD001")
		# command = input("enter g: ")
		# if command == 'g':
			# Free()
			# RequestDispence('MRGBAD001')
			# RequestStNo('81607133871')
			# logging.debug("despensing")
			# DispenceIC(1,0xB3,3)
			# time.sleep(2)
			# DispenceIC(2,0xB3,3)
			# FinalMsg()
			# 
			# DispenceIC(1,0xB5,0)
			# command = ''
		# if command == 'f':
			# Free();
		# if command == 'e':
			# mailadmin(6,0)
			# i = 0
			# for i in range (0, len(admins)):
			# 	logging.debug(admins[i])
			# 	if 'MRGBAD001' == admins[i]:
						
			# 		command = ''
			# 	i = i + 1

# except KeyboardInterrupt:
# 	logging.debug("keyboard out!")
# except:
# 	logging.warning("Unkown Error")
# finally:
# 	GPIO.cleanup()