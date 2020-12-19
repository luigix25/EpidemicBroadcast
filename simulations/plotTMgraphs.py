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

def print_graph_TM(title, values, path, yLimit = False, yStep = 5):
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

    if yLimit:
    	plt.yticks(np.arange(0, yLimit+yStep, step=yStep))
    plt.savefig(os.path.join(path, title +".png"))
    #End print_graph_TM(title, values)


def print_graph_TM_ratio(title, values_numerator, values_denominator, path):
    mean_numerator_values = {}
    mean_denominator_values = {}
    for key in  values_numerator.keys():
        mean_numerator_values[key] = np.mean(values_numerator[key])
        mean_denominator_values[key] = np.mean(values_denominator[key])

    y = []
    x = []
    key_ordered = orderkey(mean_numerator_values)
    for key in key_ordered:
        y.append(mean_numerator_values[key]/mean_denominator_values[key])
        x.append(key)


    plt.figure(figsize=(20, 10))
    plt.xticks(range(len(x)), x, size='small', rotation=90)
    #plt.errorbar(x, y, color='black', yerr=ci, fmt='o', ecolor='red', elinewidth=3, capsize=0)
    plt.title(title + " Analysis")
    plt.xlabel('T,M')
    plt.ylabel("Avg " + title)
    plt.xticks(range(len(x)), x, size='small')
    plt.grid(True)
    plt.scatter(x, y)
    plt.savefig(os.path.join(path, title +".png"))
    #End print_graph_TM_ratio(title, values)


collisionTValues        = {}
collisionFValues        = {}
receivedPacketsValues   = {}
coveredValues           = {}
simTimeValues           = {}
sendMessageValues       = {}
neighborsValues         = {}
header 					= {}

files = os.listdir(path)
#scorro tutti i file e mi salvo nella hashmap key(radius)-vector(values) tutti i valori 

for file in files:
    if file.endswith(".csv"):
        coveredValuesTmp        = []
        sendMessageValuesTmp    = []
        simTimeValuesTmp        = []

        rowFile = pd.read_csv(os.path.join(path,file))
        header = extractHeader(rowFile)
        custom_key = "t:"+header['T']+"-"+"m:"+header['m']
        #Scorro un file
        for i in range(0,len(rowFile['value'])):
            if rowFile['type'][i] != 'scalar':      #Skipping the header
                continue

            #print(rowFile['value'][i])
            if(rowFile['name'][i] == '#TrickleCollision'):
                if not custom_key in collisionTValues:
                    collisionTValues[custom_key] = []
                collisionTValues[custom_key].append(rowFile['value'][i])

            elif(rowFile['name'][i] == '#FullCollision'):
                if not custom_key in collisionFValues:
                    collisionFValues[custom_key] = []
                collisionFValues[custom_key].append(rowFile['value'][i])

            elif(rowFile['name'][i] == '#ReceivePacketInTSlots'):
                if not custom_key in receivedPacketsValues:
                    receivedPacketsValues[custom_key] = []
                receivedPacketsValues[custom_key].append(rowFile['value'][i])

            elif(rowFile['name'][i] == '#Covered'):
                coveredValuesTmp.append(rowFile['value'][i])

            elif (rowFile['name'][i] == '#FirstMessageTime[slot]'):
                simTimeValuesTmp.append(rowFile['value'][i])

            elif (rowFile['name'][i] == '#SendMessage'):
                sendMessageValuesTmp.append(rowFile['value'][i])

            elif (rowFile['name'][i] == '#Neighbors'):
                if not custom_key in neighborsValues:
                    neighborsValues[custom_key] = []
                neighborsValues[custom_key].append(rowFile['value'][i])

        #Fine File
        #Dopo ogni file aggiungo la statistica dei boolean
        sumCovered = 0
        for val in coveredValuesTmp:
            sumCovered += val
        if not custom_key in coveredValues:
            coveredValues[custom_key] = []
        coveredValues[custom_key].append(sumCovered)

        #Dopo ogni file aggiungo la statistica dei boolean
        sumSendMessage = 0
        for val in sendMessageValuesTmp:
            sumSendMessage += val
        if not custom_key in sendMessageValues:
            sendMessageValues[custom_key] = []
        sendMessageValues[custom_key].append(sumSendMessage)

        if not custom_key in simTimeValues:
            simTimeValues[custom_key] = []
        simTimeValues[custom_key].append(np.max(simTimeValuesTmp))


distribution = "normal"
if header['distributionType'] == '1':
	distribution = "gaussian"

#Genero il nome della cartella e la creo
folderTitle = "Radius(" + header['R'] + ")_Redrop(" + header['redrop'] + ")_Distribution(" + distribution + ")_Repetition(" + header['numberRepetition'] + ")_CL(" + str(confidanceLevel) + ")"
save_path = os.path.join("graph",folderTitle)
#Se la cartella esiste già la rimuovo prima
if os.path.exists(save_path):
    shutil.rmtree(save_path, ignore_errors=True)
os.mkdir(save_path)

order_key = ["t","m"]

#Stampo ordinando prima per t poi per m
for order in order_key:
    order_by = order

    print_graph_TM("TrickleCollision("+order_by+")",collisionTValues, save_path, 2, 0.2) #

    print_graph_TM("FullCollision("+order_by+")",collisionFValues, save_path, 6, 0.5) #

    print_graph_TM("ReceivedPacketsInTSlot("+order_by+")",receivedPacketsValues, save_path, 2, 0.2) #

    print_graph_TM("Covered("+order_by+")",coveredValues, save_path, 100, 5) #

    print_graph_TM("SimulationTimeSlot("+order_by+")",simTimeValues, save_path)

    print_graph_TM("SendMessage("+order_by+")",sendMessageValues, save_path, 100, 5) #

    print_graph_TM("Neighbors("+order_by+")",neighborsValues, save_path, 30, 1) #

    #print_graph_TM_ratio("TrickleCollisionOverCovered("+order_by+")", collisionTValues, coveredValues, save_path)   

    #print_graph_TM_ratio("FullCollisionOverCovered("+order_by+")", collisionFValues, coveredValues, save_path)   

    #print_graph_TM_ratio("SendMessageOverCovered(" + order_by + ")", sendMessageValues, coveredValues, save_path)

    #print_graph_TM_ratio("CollisionOverSendMessage(" + order_by + ")", collisionValues, sendMessageValues, save_path)

    #print_graph_TM_ratio("SimulationTimeOverCovered("+order_by+")", simTimeValues, coveredValues, save_path)