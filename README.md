# TCP-Client-Server

Connexion TCP client/serveur via socket

serveur_minimal :
- connexion 1 seul client

serveurfils, serveurpetitfils :
- multi clients
- connexion(s) gérée(s) par le processus fils,  ou/et petit-fils

serveurthread :
- multi clients
- connexion(s) gérée(s) par les threads

compilation :
- gcc -g -Wall -Wextra -Og -o fichier fichier.c

usage :
- usage serveur :  ./serveurname port
- usage client :   ./client_minimal localhost port
