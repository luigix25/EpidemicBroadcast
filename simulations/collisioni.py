import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
#%matplotlib inline

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
    if order_by == 't':
        for t in range(minT, maxT+1, 1):
            for m in range(minM, maxM+1, 1):
                if m <= t:
                    keyOrdered.append("t:" + str(t) + "-" + "m:" + str(m))
    elif order_by == 'm':
        for m in range(minM, maxM+1, 1):
            for t in range(minT, maxT+1, 1):
                if m <= t:
                    keyOrdered.append("t:" + str(t) + "-" + "m:" + str(m))
    else:
        print("[ERRORE] Selezionare correttamente un ordinamento dell'asse x")
        exit()
    return keyOrdered
    #End orderkey(list)

def print_graph_TM(title, values):
    mean_values = {}
    ci_values = {}
    for key in values.keys():
        curr = values[key]
        mean_values[key] = np.mean(curr)
        ci_values[key] = confidanceLevel * (np.std(curr) / np.sqrt(len(curr)))

    y = []
    x = []
    ci = []
    key_ordered = orderkey(mean_values)
    for key in key_ordered:
        y.append(mean_values[key])
        x.append(key)
        ci.append(ci_values[key])

    plt.figure(figsize=(20, 10))
    plt.xticks(range(len(x)), x, size='small', rotation=90)
    plt.errorbar(x, y, color='black', yerr=ci, fmt='o', ecolor='red', elinewidth=3, capsize=0)
    plt.title(title + " Analysis")
    plt.xlabel('T,M')
    plt.ylabel("Avg " + title)
    plt.xticks(range(len(x)), x, size='small')
    plt.grid(True)
    plt.scatter(x, y)
    plt.savefig("graph_" + title + ".png")
    #End print_graph_TM(title, values)
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




print_graph_TM("Collision",collisionValues)

print_graph_TM("ReceivedPackets",receivedPacketsValues)

print_graph_TM("Covered",coveredValues)

print_graph_TM("SimulationTime",simTimeValues)


'''
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
'''