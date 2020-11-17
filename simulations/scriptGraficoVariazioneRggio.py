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

allValues = {}
files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        rowFile = pd.read_csv(path + file)
        header = extractHeader(rowFile)
        #print(header)
        value = rowFile['value'][23]
        #print(value)
        # se la key non è ancora presente, aggiungo vettore vuoto
        if not header['radius'] in allValues:
            allValues[header['radius']] = []
        # appendo il valore corrente
        allValues[header['radius']].append(value) 


mean = {}

for key in allValues.keys():
    curr = allValues[key]
    mean[key] = np.mean(curr)

#print(mean)
radius = []
unlinked = []

for key in allValues.keys():
    unlinked.append(mean[key])    
    radius.append(float(key))

plt.title('Radius analysis')
plt.xlabel('Radius')
plt.ylabel('Unlinked nodes')
plt.xticks(np.arange(min(radius), max(radius)+50, 50.0))
plt.yticks(np.arange(min(unlinked), max(unlinked)+50, 10.0))
plt.grid(True)
plt.scatter(radius, unlinked)
plt.savefig('graph.png')