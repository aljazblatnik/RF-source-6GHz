import visa
import csv
import serial
import time
import math
from operator import itemgetter

def proper_round(num, dec=0):
    num = str(num)[:str(num).index('.')+dec+2]
    if num[-1]>='5':
      a = num[:-2-(not dec)]       # integer part
      b = int(num[-2-(not dec)])+1 # decimal part
      return float(a)+b**(-dec+1) if a and b == 10 else float(a+str(b))
    return float(num[:-1])

rm = visa.ResourceManager()
print("Najdene naprave na vodilu: ")
print(rm.list_resources())
inst = rm.open_resource('GPIB0::22::INSTR')

inst.write("AT AUTO")
inst.write("VB AUTO")
inst.write("RL 20DBM")
inst.write("CNF 4000000KHZ")
inst.write("SPF 20MHZ")
inst.write("RB 300KHZ")
#print(inst.query("MKL?")) # dobimo podatek o moci

ser = serial.Serial('COM6')

frequency = 25000

filename = "rezultat.csv"
f = open(filename, 'wt')
writer = csv.writer(f)

i = 0
najvecja = 0

while(frequency<=6500000):
    meas_result = []
    
    inst.write("CNF " + str(frequency) + "KHZ")
    string = 'F ' + str(frequency).zfill(7) + '\r\n'
    ser.write(string.encode())    
    time.sleep(0.5)
    amplitude = float(inst.query("MKL?"))
    frac, whole = math.modf(amplitude)
    print(str(frequency) + ', ' + str(amplitude))

    writer.writerow((frequency,amplitude))
    frequency = frequency + 1000

rm.close()
f.close()
