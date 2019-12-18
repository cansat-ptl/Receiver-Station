# CanSat telemetry parser
Created by Anatoly Antonov ( https://github.com/awend0 ) for the 8th Moscow State University Aerospace Engineering School 2019 (http://roscansat.com)

# How to use:
1) Run test.exe (from "test" folder), look for your COM port names and their baud rates. Example: "COM3", "Baudrate: 9600", etc.
2) Open settings.ini and fill all the fields.
3) Run Station.exe, wait for the first packet to be processed.
4) enjoy! don't waste your time, create things, happy coding.

Receiver must send data to the COM port in the following format: "CALLSIGN:[packetname]:[data];\r\n", for example:

"YKTSAT5:MAIN:N=100;ET=215;VBAT=732;ALT=540;PRS=1004323;T1=440;T2=213;\r\n"
"YKTSAT5:ORIENT:N=15;ET=27;AX=83;AY=140;AZ=-60;\r\n"
"YKTSAT5:GPS:N=12;ET=12;SAT=5;LAT=61.230;LON=129.154;ALT=200;\r\n"

# Toolchain info:
Qt 5.12.3

minGW 7.3.0 64-bit
