#!/usr/bin/python3

import serial
import struct
import time

subList = []
pubList = []

class PubInfo:
    def __init__(self) -> None:
        self.topicName = None
        self.topicId = None
        self.type = None

class SubInfo:
    def __init__(self) -> None:
        self.topicName = None
        self.topicId = None
        self.type = None

class PacketRequestTopic:
    def __init__(self, header1 = 0x05, header2 = 0x09, length = 0x02, instruction = 0x01):
        self.header1 = header1
        self.header2 = header2
        self.length = length
        self.instruction = instruction
        self.checksum = self.length + self.instruction

    def serialize(self) -> bytes:
        return struct.pack("5b", self.header1, self.header2, self.length, self.instruction, self.checksum)

class ReceivingState:
    GET_HEADER1 = 0
    GET_HEADER2 = 1
    GET_LENGTH = 2
    GET_MESSAGE = 3

def processMessage(message):
    global subList

    if(message[1] == 0x02): # no need to convert message[0] to int. When accessing the bytes variable by using []. it will automatically convert into int
        # print(f"Response Topic is received: {len(message)}. length: {message[0]}")
        # for i in message:
        #     print(i, end=" ")
        # print("")

        # calculate checksum
        checksum = 0
        for i in range(len(message) - 1):
            checksum += message[i]
        checksum &= 0xFF
        # print(f"Checksum: {checksum}")
        dt = struct.unpack(f"5B{message[0]-5}sB", message)

        if(dt[4] == 0): #Sub
            subInfo = SubInfo()
            subInfo.topicId = dt[2]
            subInfo.type = dt[3]
            subInfo.topicName = dt[5].decode()

            # print("--------- Subscribe ---------")
            # print(f"topicId     : {subInfo.topicId}")
            # print(f"type        : {subInfo.type}")
            # print(f"topicName   : {subInfo.topicName}")
            # print("--------------")
            print(f"Subscribe: {subInfo.topicName} with topicId: {subInfo.topicId}")
            subList.append(subInfo)

        if(dt[4] == 1): #Pub
            pubInfo = PubInfo()
            pubInfo.topicId = dt[2]
            pubInfo.type = dt[3]
            pubInfo.topicName = dt[5].decode()

            # print("--------- Publish ---------")
            # print(f"topicId     : {pubInfo.topicId}")
            # print(f"type        : {pubInfo.type}")
            # print(f"topicName   : {pubInfo.topicName}")
            # print("--------------")
            print(f"Publish: {pubInfo.topicName} with topicId: {pubInfo.topicId}")
            pubList.append(pubInfo)
        
        # print(dt)

# Configure the serial port settings
serial_port = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=1)

packetRequestTopic = PacketRequestTopic()

data = packetRequestTopic.serialize()
serial_port.write(data)
time.sleep(0.01)

receivingState = ReceivingState.GET_HEADER1
messageLength = 0
messageCnt = 0
message = b''

while 1:
    while(serial_port.in_waiting > 0):
        rxData = serial_port.read(1)
        # print(f"received data: {rxData}")
        if(receivingState == ReceivingState.GET_HEADER1):
            if(rxData == b'\x05'): 
                # print("Header 1 is received") 
                receivingState = ReceivingState.GET_HEADER2
        elif(receivingState == ReceivingState.GET_HEADER2):
            if(rxData == b'\x05'):  receivingState = ReceivingState.GET_HEADER2
            elif(rxData == b'\x09'): 
                receivingState = ReceivingState.GET_LENGTH
                # print("Header 2 is received")
            else: receivingState = ReceivingState.GET_HEADER1
        elif(receivingState == ReceivingState.GET_LENGTH):
            messageLength = int.from_bytes(rxData, 'little')
            # print(f"data length: {messageLength}")
            message = rxData #start message with the length data
            messageCnt = 0
            receivingState = ReceivingState.GET_MESSAGE
        elif(receivingState == ReceivingState.GET_MESSAGE):
            # print(f"m[{messageCnt}]: {rxData}")
            message += rxData
            messageCnt += 1

            if(messageCnt == messageLength):
                # process
                # print(message)
                # print(message[0], rxData[0])
                processMessage(message)

                # reset the state
                messageCnt = 0
                receivingState = ReceivingState.GET_HEADER1

    time.sleep(0.001)

serial_port.close()
