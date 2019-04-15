# Handler of CanSat data packets
Created Anatoly Antonov ( https://github.com/champi2061 ) for 8-th Russian Championship of project "Air and Engineering School" 2019

Realised features: draw charts of data from data packets(zoomable, scrollable, auto update of ranges), save them to PNG, save handled data values to CSV

Coming soon: realtime offline map with pointer of CanSat by GPS data, sending commands to guided antenna using serial port 

# How to use:
1) Run test.exe, write down your COM port name. Example: "COM3".
2) Run Station.exe, choose setting mode and enter your COM port name
3) enjoy! don't waste your time , create things, happy coding

receiver must send in COM port data packets with format "NAME:[packetname]:[data];" at 9600 baud
