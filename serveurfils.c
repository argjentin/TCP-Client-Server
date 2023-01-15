#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_BACKLOG 16
#define BUFSIZE 4096

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        return 1;
    }

    const int port = atoi(argv[1]);

     /* création de la socket */
    int lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0) {
        perror("socket");
        return 2;
    }

    /* association de la socket au port donné */
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(lsock, (struct sockaddr *)&server_addr, sizeof server_addr) != 0) {
        perror("bind");
        return 2;
    }

    /* mise en écoute de la socket */
    if (listen(lsock, SERVER_BACKLOG) != 0) {
        perror("listen");
        return 2;
    }

    while (1){
        /* attente d'un client */
        struct sockaddr_in client_addr;
        client_addr.sin_family = AF_INET;
        socklen_t client_addr_len = sizeof client_addr;
        int sock = accept(lsock, (struct sockaddr *)&client_addr, &client_addr_len);

        if (sock < 0) {
            perror("accept");
            return -1;
        }

        /* création d'un processus fils pour traiter la connexion avec le client */
        int pid = fork();
        if (pid == 0) {
            /* ce code est exécuté par le processus fils */
            close(lsock);
            printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            do
            {
                char buf[BUFSIZE];
                int n = read(sock, buf, BUFSIZE); // lecture du message envoyé par le client
                if (n < 0) {
                    perror("read");
                    return 4;
                }
                if (n == 0) {
                    printf("Client disconnected\n");
                    break;
                }
                printf("Message reçu du client: %s\n", buf);
                printf("Port du client: %d\n", ntohs(client_addr.sin_port)); 
                write(sock, buf, n);
            } while (1);

            close(sock);
            return 0;
        } else if (pid > 0) {
            /* ce code est exécuté par le processus père */
            close(sock);
            // int status;   si on veut éviter les zombies
            // waitpid(pid, &status, 0);
        } else {
            perror("fork");
            return -1;
        }
    }
}



