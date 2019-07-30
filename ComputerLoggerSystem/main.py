#!/usr/bin/env python3

# Import Modules
import serial # pip install pyserial (NOT the "serial" module available from pip)
import serial.tools.list_ports
import time, sys, os, csv

# Import My Modules
import tools

serialPortName = input("Select a serial port (Options: {}): ".format(str(tools.listSerialPorts())))
port = serial.Serial(serialPortName, 9600)

outputDischargeLogPath = "OutputLog {}.csv"

#### TO DO:
# Test the cyclical charging/discharging system (n times)
# Allow configuration of minimum battery discharge voltage from software (overwrite Arduino default)
# Allow configuration of polling frequency from Arduino
# Change Arduino code to determine charging completion by Charge Circuit LED Status (instead of by measuring voltage)

def parseLogLine(logLine):
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

	return logLineData

while 1:
	print("""\n\nSelect an Option:
1. Cyclically Discharge/Charge the battery __ times, logging.
2. FORCE: Just charge the battery.
3. FORCE: Just discharge the battery to certain voltage (for storage).
4. FORCE: Do Nothing/Idle.
8. Exit.""")
	optionSelect = input("Select Option: ")

	if optionSelect == "1":
		# Get further user settings
		cycleCount = int(input("How many times to charge/discharge the battery: "))

		# Open the logging CSV
		with open(outputDischargeLogPath.format(int(time.time())), 'w') as logCSVFile:
			fieldnames = ["TimeMS", "Voltage", "Current"]
			logDictWriter = csv.DictWriter(logCSVFile, fieldnames=fieldnames)
			logDictWriter.writeheader()

			for currentCycleCount in range(1, currentCycleCount+1):
				# Enter Charge Mode
				port.write(b"CHARGE")

				# Wait for charge to finish
				while 1:
					logLine = port.readline().decode("ascii")
					if "[LOG] " in logLine:
						logLineData = parseLogLine(logLine)

						print("\t[INFO] Currently Charging #{}/#{}: {}        ".format(currentCycleCount, cycleCount, logLineData), end="\r")

						if logLineData.get("Status", "") == "Done Charging":
							break

				# Switch to Discharge
				print("\t[INFO] Switching to Discharge with Logging")
				port.write(b"DISHARGE")

				# Log Discharge
				while 1:
					logLine = port.readline().decode("ascii")
					if "[LOG] " in logLine:
						logLineData = parseLogLine(logLine)
						logDictWriter.writerow(logLineData) # write the log line

						print("\t[INFO] Currently Discharging #{}/#{}: {}        ".format(currentCycleCount, cycleCount, logLineData), end="\r")

						if logLineData.get("Status", "") == "Done Discharging":
							break
				

	elif optionSelect == "2":
		# put arduino in charge mode
		port.write(b"CHARGE")

		print("Entering CHARGE Mode")

	elif optionSelect == "3":
		# put arduino in charge mode
		port.write(b"DISHARGE")

		print("Entering DISCHARGE Mode")

	elif optionSelect == "4":
		# put arduino in idle mode
		port.write(b"IDLE")

		print("Entering IDLE Mode")


	elif optionSelect == "8":
		# put arduino in idle mode
		port.write(b"IDLE")

		exit()