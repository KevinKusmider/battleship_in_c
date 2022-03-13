#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <arpa/inet.h> 

#define PORT 3550 
#define ADDRESS "127.0.0.1" 

//============================================================== 
//  CLIENT POUR UN SERVEUR DS UNE MACHINE DISTINCTE OU NON  
//  L'adresse IP du serveur est dans ADRESS ET LE PORT dans PORT 
//============================================================== 
 
int main() 
{   
   int sd , fromlen, retrecv; 
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
         
   // on pourrait utiliser read(sd,essai,6) au lieu de recv     
   retrecv=recv (sd,essai,39,0); 
   if (retrecv==-1) { 
      perror("\n Erreur recv : "); 
      exit(3); 
   } 

   printf("\n%s\n",essai); 
   exit(0); 
}
