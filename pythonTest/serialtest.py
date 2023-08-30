#!/usr/bin/python3

import serial
import struct
import time

class PacketRequestTopic:
    def __init__(self, header1 = 0x05, header2 = 0x09, length = 0x02, instruction = 0x01):
        self.header1 = header1
        self.header2 = header2
        self.length = length
        self.instruction = instruction
        self.checksum = self.length + self.instruction

    def serialize(self) -> bytes:
        return struct.pack("5b", self.header1, self.header2, self.length, self.instruction, self.checksum)


# Configure the serial port settings
serial_port = serial.Serial('/dev/pts/1', baudrate=9600, timeout=1)

packetRequestTopic = PacketRequestTopic()

data = packetRequestTopic.serialize()
serial_port.write(data)
time.sleep(0.01)

# data1 = packetRequestTopic.serialize()[:3]
# data2 = packetRequestTopic.serialize()[3:]
# serial_port.write(data1)
# time.sleep(0.01)
# serial_port.write(data2)

# Close the serial port
serial_port.close()
