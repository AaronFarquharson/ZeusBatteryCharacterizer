#!/usr/bin/env python3

# Import Modules
import serial # pip install pyserial (NOT the "serial" module available from pip)
import serial.tools.list_ports
import time, sys, os

# Import My Modules
import tools

serialPortName = input("Select a serial port (Options: {}): ".format(str(tools.serial_ports())))
port = serial.Serial(serialPortName, 9600)

while 1:
	print("""\n\nSelect an Option:
1. Discharge/Charge the battery __ times, logging.
2. Just charge the battery.
3. Just discharge the battery to certain voltage (for storage).
4. Do Nothing/Idle.""")
	optionSelect = input("Select Option: ")

	if optionSelect == "1":
		cycleCount = int(input("How many times to charge/discharge the battery: "))
		currentCycleCount = 1

		while 1:
			logLine = port.readline().decode("ascii")
			if "[LOG] " in logLine:
				# Read the line from the Arduino into dict logLineData (ex: {"TimeMS": something, "Voltage": something, etc.})
				logLine = logLine.replace("[LOG] ", '').replace("\n", "").replace("\r", "")

				logLine = logLine.split(", ")

				logLineData = {}
				for keyValue in logLine:
					keyValue = keyValue.split("=")
					try:
						logLineData[keyValue[0]] = float(keyValue[1])
					except ValueError:
						logLineData[keyValue[0]] = keyValue[1]

	elif optionSelect == "2":
		# put arduino in charge mode
		port.write(b"CHARGE")

		print("Entering CHARGE Mode")

	elif optionSelect == "3":
		# put arduino in charge mode
		port.write(b"DISHARGE")

		print("Entering DISCHARGE Mode")

	if optionSelect == "4":
		# put arduino in idle mode
		port.write(b"IDLE")

		print("Entering IDLE Mode")