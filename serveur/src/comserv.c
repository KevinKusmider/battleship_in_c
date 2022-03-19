#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 

int send_answer(int ns, char * type, char * content) {
    
    if(ns <= 0 || type == NULL || !strlen(type) || content == NULL) return 0;
    if((strlen(type) + strlen(content) + 3) > 500) return 0;

    char toSend[500];
    int retSend;
    sprintf(toSend, "%s:%s", type, content);

    retSend = send(ns, toSend, strlen(toSend)+1, 0);
    return retSend;
}

int listen_answer(int ns) {
    for(;;) {
        int retrecv; 
        char fullResponse[500]; 
        char type[100]; // "login:admin\0"
        char *content;

        retrecv = recv (ns, fullResponse, 500, 0);
        if (retrecv == -1) { 
            perror("\n Erreur recv : "); 
            exit(3); 
        } 
        sscanf(fullResponse, "%[^:]", type);

        content = fullResponse + strlen(type)+1;
      
        if (strcmp(type, "login") == 0) 
        {
            checkLogin(ns, content);
            continue;
        } 
    }

   return 0;
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