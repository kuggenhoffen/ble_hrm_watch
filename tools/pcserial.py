import serial
import sys
import time
import afproto
import cmd

class SerialDisplay:
	rx_buffer = ''
	counter = 0

	def __init__(self, tty, baudrate, timeo):
		self.stop = False
		self.ser = serial.Serial(tty, baudrate, timeout=timeo)
		self.last = False

	def loop(self):
		while (True):
			ch = sys.stdin.read(1)
			if (ch == ord("q")):
				break
			bytes = self.ser.read(1)
			sys.stdout.write(bytes)

	def closeSerial(self):
		self.ser.close()

	def parse(self, data):
		self.rx_buffer += data
		(packet,extra) = afproto.afproto_get_data(self.rx_buffer)
		self.rx_buffer = extra
		if packet:
			print("EXTRA: " + extra)
			print("FRAME: " + packet)

	def sendTime(self):
		newtime = int(time.mktime(time.localtime()))
		newtime = newtime + 2*60*60
		print(time.localtime())
		self.ser.write("a" + chr(5) + chr(0) + chr((newtime >> 24) & 0xFF) + chr((newtime >> 16) & 0xFF) + chr((newtime >> 8) & 0xFF) + chr((newtime) & 0xFF))
		
class SerialConsole(cmd.Cmd):
	def do_dsend(self, line):
		if (line):
			params = line.split(" ")
			str = afproto.afproto_frame_data(params[0])
			if len(params) >= 2:
				str = params[1] + str
			if len(params) >= 3:
				str = str + params[2]
			self.ser = serial.Serial("/dev/ttyUSB0", 460800, timeout=2)
			self.ser.write(str)
			extra = self.ser.read(8096)
			print extra
			self.ser.close()
		else:
			print "Missing string to send"
		
	def do_EOF(self, line):
		return True
		
	def do_exit(self, line):
		return True
		
	def do_read(self, line):
		self.ser = serial.Serial("/dev/ttyUSB0", 460800, timeout=2)
		extra = self.ser.read(8096)
		print extra
		self.ser.close()
		
	def do_send(self, line):
		if (line):
			self.ser = serial.Serial("/dev/ttyUSB0", 460800, timeout=2)
			self.ser.write(afproto.afproto_frame_data(line))
			rx = ""
			while (True):
				extra = self.ser.read(8096)
				if not extra:
					break
				rx += extra
			print rx
			extra = rx
			data = ""
			while (True):
				(new,extra) = afproto.afproto_get_data(extra)
				if not new:
					break
				data += new
			self.ser.close()
			
			f = open("exercise.log", "w")
			f.write(data)
			f.close()
			
		else:
			print "Missing string to send"
			
	def do_getfile(self, id):
		if (id):
			self.ser = serial.Serial("/dev/ttyUSB0", 460800, timeout=2)
			self.ser.write(afproto.afproto_frame_data("GETFILE:" + id))
			data = ""
			while (True):
				new = self.ser.read(1024)
				print("len" + str(len(new)))
				if not new:
					break
				self.ser.write(afproto.afproto_frame_data("ACK"))
				data += new
			
			file_data = ""
			extra = data
			while (True):
				(new,extra) = afproto.afproto_get_data(extra)
				if not new:
					break
				file_data += new.split(":")[1]
			self.ser.close()
			
			f = open("exercise.log", "w")
			f.write(file_data)
			f.close()
		else:
			print "Missing string to send"

if __name__ == "__main__":
	SerialConsole().cmdloop()
