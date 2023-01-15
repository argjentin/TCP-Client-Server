#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_BACKLOG 16
#define BUFSIZE 4096

/* Structure qui contiendra les informations sur la connexion du client */
struct client_info {
  int sock;
  struct sockaddr_in addr;
};

/* Fonction exécutée par le thread qui traite la connexion du client */
void *client_handler(void *arg) {

    // On récupère les informations sur la connexion du client
    struct client_info *info = (struct client_info *)arg;
    int sock = info->sock;
    struct sockaddr_in addr = info->addr;
    free(arg); // On libère la mémoire allouée pour la structure

    printf("Client connected: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

    do {
        char buf[BUFSIZE];
        int n = read(sock, buf, BUFSIZE);
        if (n < 0) {
            perror("read");
            break;
        }
        if (n == 0) {
            printf("Client disconnected\n");
            break;
        }
        printf("Message reçu du client: %s\n", buf);
        printf("Port du client: %d\n", ntohs(addr.sin_port)); 
        write(sock, buf, n);
    } while (1);

    close(sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    const int port = atoi(argv[1]);

    /* Création de la socket */
    int lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) {
        perror("socket");
        return 2;
    }

    /* Association de la socket au port donné */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lsock, (struct sockaddr *)&server_addr, sizeof server_addr) != 0) {
        perror("bind");
        return 2;
    }

    /* Mise en écoute de la socket */
    if (listen(lsock, SERVER_BACKLOG) != 0) {
        perror("listen");
        return 2;
    }
    while (1) {
    /* Attente d'un client */
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof client_addr;
        int sock = accept(lsock, (struct sockaddr *)&client_addr, &client_addr_len);

        if (sock < 0) {
            perror("accept");
            return 3;
        }

    /* Création d'un thread pour traiter la connexion avec le client */
        pthread_t thread;
        struct client_info *info = malloc(sizeof *info);
        info->sock = sock; // On stocke les informations sur la connexion du client
        info->addr = client_addr; // dans une structure pour les passer au thread
        int ret = pthread_create(&thread, NULL, client_handler, info);
        if (ret != 0) {
            perror("pthread_create");
            close(sock);
        } else {
            pthread_detach(thread); // On détache le thread pour qu'il se libère automatiquement
        }
    }
    close(lsock);
    return 0;
}

 
