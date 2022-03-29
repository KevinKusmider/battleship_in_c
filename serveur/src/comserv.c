#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 

#include "../headers/protoserv.h"

void show_response(RESPONSE *response) {
    printf("\nFull response : %s", response->fullResponse);
    printf("\nType : %s", response->type);
    printf("\nContent : %s\n", response->content);
}

int reset_response(RESPONSE *response) {
    strcpy(response->fullResponse, "\0");
    strcpy(response->type, "\0");
    response->content = NULL;

    return 1;  
}

int send_response(int ns, char * type, char * content) {
    
    if(ns <= 0 || type == NULL || !strlen(type) || content == NULL) return 0;
    if((strlen(type) + strlen(content) + 3) > 500) return 0;

    char toSend[500];
    int retSend;
    sprintf(toSend, "%s:%s", type, content);

    retSend = send(ns, toSend, strlen(toSend)+1, 0);
    return retSend;
}

int listen_response(int ns, RESPONSE *response) {
    reset_response(response);

    for(;;) {
        int retrecv; 

        retrecv = recv(ns, response->fullResponse, 500, 0);
        if (retrecv == -1) { 
            perror("\n Erreur recv : "); 
            exit(3); 
        } 

        sscanf(response->fullResponse, "%[^:]", response->type);

        response->content = response->fullResponse + strlen(response->type)+1;
      
        if(strlen(response->fullResponse) != 0) {
            break;
        }
    }

    return 1;
}

/**
 * @brief Vider le flux d'entrée 
 * 
 */
void clean_stdin() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}