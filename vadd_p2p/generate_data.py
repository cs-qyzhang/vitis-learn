#!/usr/bin/python3
import os
import random

data_file = "/tmp/data.bin"
result_file = "/tmp/result.bin"
data_size = 4096*32

# generate data
in1 = []
in2 = []
out = []
for i in range(0, data_size):
    in1.append(random.randint(0, 10000000))
    in2.append(random.randint(0, 10000000))
    out.append(in1[i] + in2[i])

# write data to file
with open(data_file, "wb") as f:
    for i in range(0, data_size):
        f.write(in1[i].to_bytes(4, byteorder='little'))
    for i in range(0, data_size):
        f.write(in2[i].to_bytes(4, byteorder='little'))

# write result to file
with open(result_file, "wb") as f:
    for i in range(0, data_size):
        f.write(out[i].to_bytes(4, byteorder='little'))
