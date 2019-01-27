#!/usr/bin/python3
"""Python script to find cheat codes for the Theodore emulator.
Requirements: several save state files corresponding to different values of a data (e.g. number of lifes...).
Usage: ./find_cheat save1.state value1 save2.state value2 save3.state value3 (...)
The script then prints the location(s) in RAM where value1 was found in save1.state
and value2 was found in save2.state at the same location(s) and ..."""

import sys
from collections import Counter

RAM_OFFSET = 701
RAM_SIZE = 0x80000

positions=[]

def find_in_file(filename, value):
	print("Searching for value " + str(value) + " in save state " + filename)
	found = []
	with open(filename, "rb") as f:
		f.read(RAM_OFFSET)
		offset = 0
		byte = f.read(1)
		while byte != b"" and offset < RAM_SIZE:
			if int.from_bytes(byte, byteorder='big') == int(value):
				found += [offset]
			offset += 1
			byte = f.read(1)
	return found

for i in range(1, len(sys.argv), 2):
	positions += find_in_file(sys.argv[i], sys.argv[i+1])

nb_save_states = (len(sys.argv) - 1) // 2
counter = Counter(positions)
print("Matching RAM addresses:")
for address, occurences in counter.items():
	if occurences == nb_save_states:
		print(hex(address))

