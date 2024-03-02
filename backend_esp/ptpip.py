import socket
import struct
import sys
from time import sleep

command = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
command.connect(("192.168.1.2", 15740))

packet = struct.pack("<II16s14sI", 42, 0x01, bytes([0x77] * 16), b"P\0y\0t\0h\0o\0n\0\0\0", 0x010000)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack, conn_number = struct.unpack("<II", command.recv(4096)[4:12])
if ack != 0x02:
    print("error")
    sys.exit(1)

print(f"Connection number: {conn_number}")

event = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
event.connect(("192.168.1.2", 15740))

packet = struct.pack("<III", 12, 0x03, conn_number)
print(' '.join('{:02x}'.format(x) for x in packet))
event.sendall(packet)

ack = event.recv(4096)
if ack[4] != 0x04:
    print("error")
    sys.exit(1)

packet = struct.pack("<IIIHII", 22, 0x06, 0x01, 0x1002, 0x0, 0x1)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

packet = struct.pack("<IIIHII", 22, 0x06, 0x01, 0x9114, 0x1, 0x01)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

packet = struct.pack("<IIIHII", 22, 0x06, 0x01, 0x9115, 0x2, 0x01)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

sleep(5)

packet = struct.pack("<IIIHIII", 26, 0x06, 0x01, 0x9128, 0x3, 0x01, 0x0)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

sleep(0.5)

packet = struct.pack("<IIIHIII", 26, 0x06, 0x01, 0x9128, 0x4, 0x02, 0x0)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

sleep(0.5)

packet = struct.pack("<IIIHII", 22, 0x06, 0x01, 0x9129, 0x5, 0x02)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

sleep(0.5)

packet = struct.pack("<IIIHII", 22, 0x06, 0x01, 0x9129, 0x6, 0x01)
print(' '.join('{:02x}'.format(x) for x in packet))
command.sendall(packet)

ack = command.recv(4096)
if struct.unpack("<H", ack[8:10])[0] != 0x2001:
    print("error")
    sys.exit(1)

try:
    input()
except:
    event.close()
    command.close()