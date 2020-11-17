import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#%matplotlib inline

path = 'results/'

#Estrae l'header dal file
def extractHeader(rowFile):
    # Riga nel file, nome che do alla chiave nella hasmap che poi ritorno
    names = {21:"y",20:"x",19:"redrop",18: "radius",17: "m",16: "T",15:"slotSize",14:"users",13:"seed"}

    ret = {}

    matrix = rowFile['attrvalue']

    for key in names.keys():
        ret[names[key]] = matrix[key]
        
    return ret;

collisionValues = {}
receivedPacketsValues = {}

files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        rowFile = pd.read_csv(path + file)
        #print(rowFile)

        header = extractHeader(rowFile)
        custom_key = header['m']+':'+header['T']

        #22 sarebbe simTime
        for i in range(23,len(rowFile['value'])):
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
    print(meanCollision[key])

for key in receivedPacketsValues.keys():
    curr = receivedPacketsValues[key]
    #print(curr)
    meanReceivedPackets[key] = np.mean(curr)
    #print(meanReceivedPackets[key])

y_values = []
x_values = []

for key in collisionValues.keys():
    y_values.append(meanCollision[key])    
    x_values.append(key)

#for key in meanCollision.keys():
#    y_values.append(meanCollision[key])    
#    x_values.append(key)

plt.title('Radius analysis')
plt.xlabel('Radius')
plt.ylabel('Unlinked nodes')
#plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
#plt.yticks(np.arange(min(y_values), max(y_values)+50, 10.0))
plt.grid(True)
plt.scatter(x_values, y_values)
plt.savefig('graph_collisions.png')