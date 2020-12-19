import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import shutil

confidanceLevel = 1.96
order_by = 'm' # t or m, ordinamento asse x
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

def print_graph(values, filename, yname):
    mean_values = {}
    ci_values = {}

    for key in values.keys():
        curr = values[key]
        mean_values[key] = np.mean(curr)
        ci_values[key] = confidanceLevel * (np.std(curr) / np.sqrt(len(curr)))

    y = []
    x = []
    ci = []

    allKeys  = []
    orderedKey = []

    for key in mean_values.keys():
    	allKeys.append(int(key))

    maxKey  = max(allKeys)
    minKey  = min(allKeys)
    stepKey = int((maxKey-minKey)/(len(allKeys)-1))

    for currentKey in range(minKey, maxKey+1, stepKey):
        orderedKey.append(currentKey)

    for keyInt in orderedKey:
        key = str(keyInt)
        y.append(mean_values[key])
        x.append(key)
        ci.append(ci_values[key])

    plt.figure(figsize=(20, 10))

    #plt.xticks(np.arange(minKey, maxKey+1, step=1), rotation=90)
    #plt.yticks(np.arange(0, 100+1, step=5))

    plt.xticks(np.arange(0, 1000+60, step=1), rotation=90)
    plt.yticks(np.arange(0, 100+1, step=5))

    plt.errorbar(x, y, color='black', yerr=ci, fmt='o', ecolor='red', elinewidth=3, capsize=0)

    plt.title("Radius Analysis")
    plt.xlabel("Radius")
    plt.ylabel(yname)

    plt.grid(True)
    plt.scatter(x, y)
    plt.savefig(filename)
    #End print_graph(title, values)


radiusValues        = {}
neighborsValues		= {}
header 				= {}
files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        coveredValuesTmp        = []
        sendMessageValuesTmp    = []
        simTimeValuesTmp        = []

        rowFile = pd.read_csv(os.path.join(path,file))
        header = extractHeader(rowFile)
        custom_key = header['R']
        #Scorro un file
        for i in range(0,len(rowFile['value'])):
            if rowFile['type'][i] != 'scalar':      #Skipping the header
                continue

            #print(rowFile['value'][i])
            if(rowFile['name'][i] == '#unlinkedNodes'):
                if not custom_key in radiusValues:
                    radiusValues[custom_key] = []
                radiusValues[custom_key].append(rowFile['value'][i])
                
            elif (rowFile['name'][i] == '#Neighbors'):
                if not custom_key in neighborsValues:
                    neighborsValues[custom_key] = []
                neighborsValues[custom_key].append(rowFile['value'][i])

        #Fine File
#Fine Lettura File

#Genero il nome della cartella e la creo
distribution = "uniform"
if header['distributionType'] == '1':
	distribution = "gaussian"

fileTitle = "Unlinked___Distribution(" + distribution + ")_Repetition(" + header['numberRepetition'] + ")_CL(" + str(confidanceLevel) + ").png"
save_path = os.path.join("graph","RadiusAnalysis")
save_path = os.path.join(save_path,fileTitle)

#print_graph(radiusValues, save_path, "Avg Users Unlinked")

fileTitle = "Neighbours___Distribution(" + distribution + ")_Repetition(" + header['numberRepetition'] + ")_CL(" + str(confidanceLevel) + ")_Redrop(" + header['redrop'] + ").png"
save_path = os.path.join("graph","RadiusAnalysis")
save_path = os.path.join(save_path,fileTitle)

print_graph(neighborsValues, save_path, "Avg Neighbors per User")
