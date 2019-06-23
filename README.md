# Handler of CanSat data packets
Created by Anatoly Antonov ( https://github.com/champi2061 ) for 8-th Russian Championship of project "Air Engineering School" 2019 ( http://roscansat.com )

# How to use:
1) Run test.exe (from "test" folder), look for your COM ports names and its baud rate. Example: "COM3" , "Baudrate: 9600" , etc.
2) Open settings.ini and fill all fields.
3) Run Station.exe, wait for first packet to be handled.
4) enjoy! don't waste your time , create things, happy coding.

receiver must send in COM port data packets with format "NAME:[packetname]:[data];\r\n", example:

"YKTSAT5:MAIN:N=100;ET=215;VBAT=732;ALT=540;PRS=1004323;T1=440;T2=213;\r\n"
"YKTSAT5:ORIENT:N=15;ET=27;AX=83;AY=140;AZ=-60;\r\n"
"YKTSAT5:GPS:N=12;ET=12;SAT=5;LAT=61.230;LON=129.154;ALT=200;\r\n"

