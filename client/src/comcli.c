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

void sendLogin(int ns) {
   char login[255];
   printf("\nLogin ? ");
   scanf("%s", login);

   send_answer(ns, "login", login);
}
 
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
      char type[100];
      char content [400];

      retrecv = recv (ns, fullResponse, 500, 0);
      if (retrecv == -1) { 
         perror("\n Erreur recv : "); 
         exit(3); 
      } 
      sscanf(fullResponse, "%[^:]", type);

            
      if (strcmp(type, "login") == 0) 
      {
         sendLogin(ns);
         continue;
      } 
   }

   return 0;
}


int main() 
{   
   int sd, fromlen, retrecv; 
   char essai[39]; 
   struct sockaddr_in   dest_addr ; 
       
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
         
   while(1) {
      listen_answer(sd);
   }   
   
   exit(0); 
}
