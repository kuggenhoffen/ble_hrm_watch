import serial
import sys
from Tkinter import Tk, Canvas, PhotoImage
import time

class SerialDisplay:
	WIDTH = 128
	HEIGHT = 128
	cury = 0
	state = 0

	def __init__(self, tty, baudrate, timeo):
		self.stop = False
		self.initUI()
		self.ser = serial.Serial(tty, baudrate, timeout=timeo)
		self.ser.write("N")
		self.last = False

	def loop(self, ):
		while (True):
			line = self.ser.readline()
			if not line:
				return
			self.parseLine(line)

	def closeSerial(self):
		self.ser.close()

	def parseLine(self, line):
		if ("START" in line):
			self.cury = 0
			print("START")
			self.state = 1
		elif ("END" in line):
			print("END")
			time.sleep(2)
			self.ser.write("N")
			self.state = 0
		elif (line[0] == '>'):
			self.last = True
			self.parseBinaryLine(line[1:])
			#print(pixels)
			if self.cury < self.HEIGHT:
				self.cury += 1
			self.window.update()
			#if (self.cury % 2 == 0):
			#	self.ser.write("N")
		else:
			print(line);

	def parseBinaryLine(self, line):
			# First xor with 0xAA to get rid of most 0's, since most of the
			# picture will probably be empty
			# byte ^= 0xAA;
			# Escape null bytes and escape bytes
			# if (byte == 0 || byte == 0x7D) {
			#	// Write escape byte
			#	*bufPtr++ = 0x7d;
			#	*bufPtr++ = byte ^ 0xBB;
			# }
			# *bufPtr++ = byte;
			# byte = 0;
			curx = 0
			escape = False;
			for char in line:
				char = ord(char)
				if char == 0x0D:
					return
				# Check if character is escape character
				if char == 0x7D and not escape:
					escape = True
					continue

				if escape:
					escape = False
					# XOR data with 0xBB
					char ^= 0xBB

				# Finally XOR data with 0xAA to get real data
				char ^= 0xAA
				for i in range(0, 8):
					if char & (1 << i):
						self.img.put("#ffffff", (curx,self.cury))
					else:
						self.img.put("#000000", (curx,self.cury))
					curx += 1
		

	def initUI(self):
		self.window = Tk()
		self.canvas = Canvas(self.window, width=self.WIDTH, height=self.HEIGHT, bg="#000000")
		self.canvas.pack()
		self.img = PhotoImage(width=self.WIDTH, height=self.HEIGHT)
		self.canvas.create_image((self.WIDTH/2, self.HEIGHT/2), image=self.img, state="normal")

print("start")
sd = SerialDisplay('/dev/ttyACM0', 460800, 5)
sd.loop()
print('Done!')
