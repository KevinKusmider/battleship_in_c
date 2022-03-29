#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <arpa/inet.h> 

#include "../headers/protocomcli.h"

#define PORT 3550 
#define ADDRESS "127.0.0.1" 

//============================================================== 
//  CLIENT POUR UN SERVEUR DS UNE MACHINE DISTINCTE OU NON  
//  L'adresse IP du serveur est dans ADRESS ET LE PORT dans PORT 
//============================================================== 

int send_asked_str(int ns, char *question, char *type) {
   char content[255];
   printf("\n%s ", question);
   scanf("%s", content);

   send_response(ns, type, content);
   clean_stdin();

   return 1;
}
 
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


int main() 
{   
   int sd, fromlen, retrecv; 
   struct sockaddr_in   dest_addr ; 
   int playing = 1;

   if((sd=socket(AF_INET,SOCK_STREAM,0)) == -1) { 
      perror("\n Erreur socket : "); 
      exit(1); 
   } 
 
   dest_addr.sin_family=AF_INET; 
   dest_addr.sin_port=htons(PORT); 
   dest_addr.sin_addr.s_addr = inet_addr(ADDRESS); 
   bzero(&(dest_addr.sin_zero),8); 

   if((connect(sd,(struct sockaddr * )&dest_addr,sizeof(struct sockaddr)))== -1) { 
      perror("\n Erreur connect : "); 
      exit(2); 
   } 
         
   RESPONSE *response;
   if(NULL == (response = malloc(sizeof(RESPONSE)))) {
      exit(6);
   }

   while(playing) {
      listen_response(sd, response);
      show_response(response);

      if(!strcmp(response->type, "login")) {
         send_asked_str(sd, "Login ?", "login");
      }

      if(!strcmp(response->type, "password")) {
         send_asked_str(sd, "Password ?", "password");
      }

      if(!strcmp(response->type, "logged")) {
         printf("\nWaiting for the game to start");
      }

      // Admin
      if(!strcmp(response->type, "show_admin_menu")) {
         printf("\nconnected as : ADMIN\n\n 1. Show users\n 2. Create User\n\n");
         char action[1] = "\0";
         printf("What do you want to do ? ");
         scanf("%c", action);
         send_response(sd, "admin_request", action);
      }
   }   
   
   exit(0); 
}
