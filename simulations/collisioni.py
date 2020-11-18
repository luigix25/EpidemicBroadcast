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
coveredValues = {}
simTimeValues = {}

files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        coveredValuesTmp = []
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

            elif(rowFile['name'][i] == '#Covered'):
                coveredValuesTmp.append(rowFile['value'][i])


            elif (rowFile['name'][i] == '#SimTime[ms]'):
                if not custom_key in simTimeValues:
                    simTimeValues[custom_key] = []
                simTimeValues[custom_key].append(rowFile['value'][i])
        sum = 0
        for val in coveredValuesTmp:
            sum += val
        if not custom_key in coveredValues:
            coveredValues[custom_key] = []
        coveredValues[custom_key].append(sum)



# COLLISION
meanCollision = {}
ciCollision = {}
for key in collisionValues.keys():
    curr = collisionValues[key]
    meanCollision[key] = np.mean(curr)
    ciCollision[key] = 1.96 * (np.std(curr) / np.sqrt(len(curr)))
    #print(meanCollision[key])



y_values = []
x_values = []
ci = []
keyOrdered1 = orderkey(meanCollision)
for key in keyOrdered1:
    y_values.append(meanCollision[key])    
    x_values.append(key)
    ci.append(ciCollision[key])

plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.errorbar(x_values, y_values, color='black', yerr=ci, fmt='o',ecolor='red', elinewidth=3, capsize=0)
plt.title('Collision Analysis')
plt.xlabel('T,M')
plt.ylabel('Avg Collisions')
plt.xticks(range(len(x_values)), x_values, size='small')
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
#plt.fill_between(x_values, (y_values-ci), (y_values+ci), color='b', alpha=.1)
plt.savefig('graph_collisions.png')


#RECEIVED PACKETS
ciReceivedPackets = {}
meanReceivedPackets = {}
for key in receivedPacketsValues.keys():
    curr = receivedPacketsValues[key]
    meanReceivedPackets[key] = np.mean(curr)
    ciReceivedPackets[key] = 1.96 * (np.std(curr) / np.sqrt(len(curr)))

y_values = []
x_values = []
ci = []

keyOrdered2 = orderkey(meanReceivedPackets)

for key in keyOrdered2:
    y_values.append(meanReceivedPackets[key])
    x_values.append(key)
    ci.append(ciReceivedPackets[key])


plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.errorbar(x_values, y_values, color='black', yerr=ci, fmt='o',ecolor='red', elinewidth=3, capsize=0)
plt.title('Received Packets Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg Packets Received')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_packets.png')


#COVERED
ciCovered = {}
meanCovered = {}
for key in coveredValues.keys():
    curr = coveredValues[key]
    meanCovered[key] = np.mean(curr)
    #print(np.mean(curr))
    ciCovered[key] = 1.96 * (np.std(curr) / np.sqrt(len(curr)))

y_values = []
x_values = []
ci = []

keyOrdered3 = orderkey(meanCovered)

for key in keyOrdered3:
    y_values.append(meanCovered[key])
    x_values.append(key)
    ci.append(ciCovered[key])


plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.errorbar(x_values, y_values, color='black', yerr=ci, fmt='o',ecolor='red', elinewidth=3, capsize=0)
plt.title('Covered Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg Covered')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_covered.png')

#COLLISION / COVERED

y_values = []
x_values = []

keyOrdered3 = orderkey(meanCovered)

for key in keyOrdered3:
    y_values.append(meanCollision[key]/meanCovered[key])
    x_values.append(key)



plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)

plt.title('Collision/Covered Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg Collision/Covered')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_collisionOverCovered.png')

#SIMTIME
ciSimTime = {}
meanSimTime = {}
for key in simTimeValues.keys():
    curr = simTimeValues[key]
    meanSimTime[key] = np.mean(curr)
    ciSimTime[key] = 1.96 * (np.std(curr) / np.sqrt(len(curr)))
y_values = []
x_values = []
ci = []

keyOrdered3 = orderkey(meanCovered)

for key in keyOrdered3:
    y_values.append(meanSimTime[key])
    x_values.append(key)
    ci.append(ciSimTime[key])



plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)
plt.errorbar(x_values, y_values, color='black', yerr=ci, fmt='o',ecolor='red', elinewidth=3, capsize=0)
plt.title('Simulation Time Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg Simulation Time [ms]')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_simTime.png')

#SIMTIME / COVERED

y_values = []
x_values = []

keyOrdered3 = orderkey(meanCovered)

for key in keyOrdered3:
    y_values.append(meanSimTime[key]/meanCovered[key])
    x_values.append(key)

plt.figure(figsize=(20,10))
plt.xticks(range(len(x_values)), x_values, size='small',rotation=90)

plt.title('SimTime/Covered Analysis')
plt.xlabel('t,m')
plt.ylabel('Avg SimTime/Covered')
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_simTimeOverCovered.png')