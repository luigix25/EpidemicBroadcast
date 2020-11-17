#!/bin/sh

for file in $(ls -1 ./results/ | grep .sca); do
	scavetool x ./results/$file -o ./results/$file.csv
done

rm -f ./results/*.sca
