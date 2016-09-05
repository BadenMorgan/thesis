import serial
import time
import pymssql
import _mssql
import decimal
import uuid
import httplib2
import urllib.request

port = 'COM5'

print("Device setup")
print("Which COM port do you want to use?")
port = input()

timeout = 9000;#seconds
currentTimeout = timeout
exitFlag = 0
tagLength = 14
value = 0

h = httplib2.Http()

print ("Opening connection to server...")

try:
    conn = pymssql.connect('srvwinsqlvs007.wf.uct.ac.za\DW', 'USERNAME', 'Password', 'DB400_reports')  #Username and password missing
    cursor = conn.cursor()

except:
    print()
    print ("ERROR! Connection failed.")
    print ("Please ensure a valid network connection and try again.")
    input ("Press Enter to continue...")
    raise SystemExit
	
print ("Done!")
print ()
print ("Opening serial connection to card reader...")

try:
    ser = serial.Serial(port,baudrate=9600,parity=serial.PARITY_ODD,stopbits=serial.STOPBITS_TWO,bytesize=serial.SEVENBITS)
except:
    print()
    print ("ERROR! Could not open serial connection.")
    print()
    print ("Please ensure that the device is plugged in")
    print ("and that you've selected the correct COM port.")
    print()
    input ("Press Enter to continue...")
    raise SystemExit
	
print ("Done!")
print ()
print("Access Logger Started and Ready")

print("Version 1.1")
print()
print("*****************************************")
print("Please scan a card.")
print()

while(exitFlag == 0):
    if(ser.inWaiting()>0):
        currentTimeout = timeout;
        if(ser.inWaiting()%tagLength == 0):
            value = ser.read(tagLength)
            print("Reading card...")
            value = value.decode("utf-8")
            value = int(value[1:-3],16)
            print(value)

            print()
            print ("Please remove card!")

            while(ser.inWaiting() > 0):
                print("Flushing extra reads...")
                ser.read(ser.inWaiting()) #flushing the system.
                print("Done!")

            print()
            time.sleep(2) #Is this necessary?
            print ("Contacting server...")   
            sqlquery = """EXEC DB400_reports.dbo.GET_TAG_DETAILS @TAG_CODE = %s""" % value
			
            try:
                cursor.execute(sqlquery)
                info = cursor.fetchone()
                print ("Done!")
                print()
                print ("Information returned:")
                print(info)
                time.sleep(1)
                message = "https://docs.google.com/forms/d/1RDIT_L9CcSgXU_3IhRvSBAhyXqSZgi2CVVz1ZR9tjeM/formResponse?ifq&entry.1459435906=%s&entry.1882114143=%s&entry.1276607781=%s&entry.922022873=%s&entry.1244811730=%s&entry.1033324385=%s&submit=Submit"
                message = message % ("'"+info[0],info[1],info[2],info[3],info[4],info[5])
                message = message.replace (" ", "%20")
                print ()
                print("Sending log to Google:")
                print()
                print(message)
                print()
                f = urllib.request.urlopen(message)
                print ("Done!")
                print()

                time.sleep(1)

                print("*****************************************")
                print("Ready for next card")
                print("")

            except:

                print("ERROR!!! Couldn't communicate with database.")
                print("Waiting 5 seconds...")
                time.sleep(5)
                print("Re-opening connection...")
              
                try:
                    conn = pymssql.connect('srvwinsqlvs007.wf.uct.ac.za\DW', 'uctaccessreports', 'uct@cc3$$r3p0rt$', 'DB400_reports')
                    cursor = conn.cursor()

                except:
                    print ("Connection failed.  Please ensure a valid network connection and try again.")
                    input ("Press Enter to continue...")
                    raise SystemExit

                print ("Done!  Please scan last card again.")
                print ()

        elif (ser.inWaiting() > tagLength):
            print("Too much data in buffer - flushing")
            time.sleep(2)
            print(ser.inWaiting())
            print(ser.read(ser.inWaiting())) #flushing the system.
            print("Flushed")

    else:
        time.sleep(1)
        currentTimeout-=1;
		
ser.close()