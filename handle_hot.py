import os

data = []
with open("/home/liyi/lixu/hot.txt", "r") as f:
    for line in f.readlines():
        addr = line.split(" ")[0]
        addr = addr.strip("\n")
        print(addr)
        #data.append(addr)
        #print(line.split(" ")[5])
    
    #print(data)
