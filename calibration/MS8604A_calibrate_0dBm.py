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
    string = 'Att ' + str(0).zfill(3) + '\r\n'
    ser.write(string.encode()) 
    time.sleep(0.5)
    amplitude = float(inst.query("MKL?"))
    frac, whole = math.modf(amplitude)
    najvecja = whole
    ostanek = amplitude - najvecja
    meas_result.append((najvecja,abs(ostanek),0));
    
    string = 'Att ' + str(1).zfill(3) + '\r\n'
    ser.write(string.encode()) 
    time.sleep(0.5)
    amplitude = float(inst.query("MKL?"))
    frac, whole = math.modf(amplitude)
    ostanek = amplitude - najvecja
    meas_result.append((najvecja,abs(ostanek),1));

    string = 'Att ' + str(2).zfill(3) + '\r\n'
    ser.write(string.encode()) 
    time.sleep(0.5)
    amplitude = float(inst.query("MKL?"))
    frac, whole = math.modf(amplitude)
    ostanek = amplitude - najvecja
    meas_result.append((najvecja,abs(ostanek),2));

    string = 'Att ' + str(3).zfill(3) + '\r\n'
    ser.write(string.encode()) 
    time.sleep(0.5)
    amplitude = float(inst.query("MKL?"))
    frac, whole = math.modf(amplitude)
    ostanek = amplitude - najvecja
    meas_result.append((najvecja,abs(ostanek),3));

    meas_result_sorted = sorted(meas_result, key = lambda x: float(x[1]))

    # vzamemo tistega z najmanjso napako
    maxValue = proper_round(meas_result_sorted[0][0]+meas_result_sorted[0][1])
    attenuatorSettings = meas_result_sorted[0][2]
    error = meas_result_sorted[0][1]
    error1 = meas_result_sorted[1][1]
    error2 = meas_result_sorted[2][1]
    error3 = meas_result_sorted[3][1]

    print('{' + str(int(maxValue)) + ',' + str(attenuatorSettings) + '},', end = "")
    i = i + 1
    if(i==10):
        print(' ')
        i = 0

    writer.writerow((frequency,int(maxValue),attenuatorSettings))
    frequency = frequency + 2000

rm.close()
f.close()
