import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#%matplotlib inline

path = 'results'

def extractHeader(rowFile):
    names = {}

    for i in range(0,50):           #hardcoded, but enough room for other attributes 
        if rowFile['type'][i] != "param":
            continue

        name = rowFile['attrname'][i]
        name = name.replace("**.","")
        name = name.replace("EpidemicBroadcast.","")

        names[name] = rowFile['attrvalue'][i]
    return names

def orderkey(list):
    tList = []
    mList = []
    # Cerco gli intervalli dei valori t ed m
    for key in list.keys():
        splitted = key.split("-")
        tList.append(int(splitted[0].replace("t:","")))
        mList.append(int(splitted[1].replace("m:","")))

    minT = min(tList)
    maxT = max(tList)
    minM = min(mList)
    maxM = max(mList)


    keyOrdered = []
    # Genero i label ordinati
    for t in range(minT, maxT+1, 1):
        for m in range(minM, maxM+1, 1):
            if m <= t:
                keyOrdered.append("t:" + str(t) + "-" + "m:" + str(m))
    return keyOrdered

collisionValues = {}
receivedPacketsValues = {}

files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        rowFile = pd.read_csv(os.path.join(path,file))
        #print(rowFile)

        header = extractHeader(rowFile)

        custom_key = "t:"+header['T']+"-"+"m:"+header['m']

        for i in range(0,len(rowFile['value'])):
            if rowFile['type'][i] != 'scalar':      #Skipping the header
                continue

            #print(rowFile['value'][i])
            if(rowFile['name'][i] == '#Collision'):
                if not custom_key in collisionValues:
                    collisionValues[custom_key] = []

                collisionValues[custom_key].append(rowFile['value'][i])
            elif(rowFile['name'][i] == '#ReceivePacketInTSlots'):
                if not custom_key in receivedPacketsValues:
                    receivedPacketsValues[custom_key] = []

                receivedPacketsValues[custom_key].append(rowFile['value'][i])


meanCollision = {}
meanReceivedPackets = {}

for key in collisionValues.keys():
    curr = collisionValues[key]
    meanCollision[key] = np.mean(curr)
    #print(meanCollision[key])



y_values = []
x_values = []
keyOrdered1 = orderkey(meanCollision)
for key in keyOrdered1:
    y_values.append(meanCollision[key])    
    x_values.append(key)

plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.title('Collision Analysis')
plt.xlabel('T,M')
plt.ylabel('Avg Collisions')
plt.xticks(range(len(x_values)), x_values, size='small')
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_collisions.png')




for key in receivedPacketsValues.keys():
    curr = receivedPacketsValues[key]
    meanReceivedPackets[key] = np.mean(curr)

y_values = []
x_values = []

keyOrdered2 = orderkey(meanReceivedPackets)

for key in keyOrdered2:
    y_values.append(meanReceivedPackets[key])
    x_values.append(key)


plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.title('Received Packets Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg Packets Received')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_packets.png')

