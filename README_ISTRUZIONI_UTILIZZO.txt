1 Eseguire la simulazione in Omnet++

2 Una volta eseguita la simulazione in omnet, lanciare lo script /simulation/script.sh
	(in questo modo lo scavetool converte i file .sca in .csv)

3 Una volta ottenuti i file .csv è possibile utilizzare uno dei seguenti script python a seconda della necessità:
	a. simulations/plotUnlinked.py per generare i grafici dei nodi unlinked e dei vicini al variare del raggio
	b. simulations/plotTMgraphs.py per generare tutti i grafici al variare di T ed m 
	
	i grafici verranno esportati nella cartella graph