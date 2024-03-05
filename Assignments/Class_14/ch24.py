# chapter 24 in python

# requires pyserial
# requires matplotlib

import serial
import sys

# your com port name
ser = serial.Serial("/dev/ttyUSB0", 230400)
print("Opening port: ")
print(ser.name)

# send 2 gains
ser.write(b"0.1 0.01\n")  # Kp Ki

# read the data back as int in reverse order
sampnum = 0
read_samples = 10
ADCval = []
ref = []
oc = []
eint = []
while read_samples > 1:
    data_read = ser.read_until(b"\n", 50)
    data_text = str(data_read, "utf-8")
    data = list(map(int, data_text.split()))

    if len(data) == 3:
        read_samples = data[0]
        ADCval.append(data[1])
        ref.append(data[2])
        sampnum = sampnum + 1

# plot it
import matplotlib.pyplot as plt

t = range(len(ADCval))  # time array
plt.plot(t, ADCval, "r*-", label="ADC Value")
plt.plot(t, ref, "b*-", label="Reference Value")
plt.ylabel("value")
plt.xlabel("sample")
# plt.legend("ADC Value", "Reference Value")
plt.legend()
plt.show()
# plt.savefig("hw_src/24_7.png")

# be sure to close the port
ser.close()
