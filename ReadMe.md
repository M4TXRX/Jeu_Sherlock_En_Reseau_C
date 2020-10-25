## Table of contents
* [General info](#general-info)
* [Setup](#setup)

## General info 
Ce jeu en réseau (client/serveur) est comparable au cluedo. L'objectif étant de démasquer le coupable par
une série de questions. 

Programmer ce jeu m'aura permis de découvrir la mise en place d'un programme client serveur en C.

Vous pouvez trouver davantage de détails sur la partie code et sur comment lancer le jeu dans "Rapport_projet.pdf".
	
## Setup

Allez dans le répertoire "Code_C" :


* Pour compiler le fichier server.c :
		```
		$ gcc -o server server.c 
		```
* Le compilateur creera un fichier executable server
	- Pour lancer le programme tapez :
		```
	  $ ./server <Numéro de port Server>
		```
		
* Pour compiler le fichier sh13.c (client) :
		```
		$ gcc -o sh13 sh13.c
		```
* Le compilateur creera un fichier executable client (lancez autant d'instances qu'il y a de joueurs)
	- Pour lancer le programme tapez :
		```
		 $ ./sh13 <IP server> <Port server> <IP Client> <Port Client> <Nom du joueur>
		```

