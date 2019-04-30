# Handler of CanSat data packets
Created by Anatoly Antonov ( https://github.com/champi2061 ) for 8-th Russian Championship of project "Air and Engineering School" 2019

Realised features: draw charts of data from data packets(zoomable, scrollable, auto update of ranges), save them to PNG, save handled data values to CSV

Coming soon: realtime offline map with pointer of CanSat by GPS data, guiding antenna using serial port and telemetry/GPS data

# How to use:
1) Run test.exe, write down your COM port name and settings. Example: "COM3" , "Baudrate: 9600" , etc.
2) Run Station.exe, and enter your COM port name and settings.
3) enjoy! don't waste your time , create things, happy coding.

receiver must send in COM port data packets with format "NAME:[packetname]:[data];", example:
YKTSAT5:MAIN:N=100;ET=215;VBAT=732;ALT=540;PRS=1004323;T1=440;T2=213;\r\n

YKTSAT5:ORIENT:N=1;AX=1-100;AY=1-100;AZ=1-100;\r\n

