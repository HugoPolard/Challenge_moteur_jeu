#! /bin/sh 

MAX=$1
if [ $# -ne 1 ]; then 
	MAX=3
fi

numJ=1
while [ $numJ -le $MAX ]
do
	echo "Creation du joueur $numJ"
	./joueur$numJ-static "Joueur $numJ" &
	numJ=$(expr $numJ + 1)
done
