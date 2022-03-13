#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 

#define NBCLI 3 
#define PORT 3550 
#define ADDRESS "127.0.0.1" 

//============================================================== 
//  SERVEUR MULTICLIENT (CHAQUE CLIENT EST DS UNE MACHINE      = 
//           DISTINCTE DU SERVEUR)                             = 
//  L'adresse IP du serveur est dans ADRESSE le port dans PORT = 
//============================================================== 
int main() 
{     
  int sd, ns , fromlen,i,retfork,retsend;

  struct sockaddr_in my_addr, user_addr ; 
 
  char *buffer= "Bienvenue sur le multiclient en reseau";      
  i=0; 
     
  if((sd=socket(AF_INET,SOCK_STREAM,0)) == -1) { 
    perror("\n Erreur socket : \n"); 
    exit(1); 
  } 

  my_addr.sin_family = AF_INET ; 
  my_addr.sin_port = htons(PORT); 
  my_addr.sin_addr.s_addr = inet_addr(ADDRESS); 
  bzero(&(my_addr.sin_zero),8); 

  if((bind(sd,(struct sockaddr * ) &my_addr,sizeof(struct sockaddr))) == -1) { 
    perror("\n Erreur bind: \n"); 
    exit(2); 
  } 
 
 
  if((listen(sd,NBCLI))== -1) { 
    perror("\n Erreur listen : \n"); exit(3); 
  } 
 
  for(;;) {   
    printf("\n i= %d Avant accept \n",i);
    ns=accept(sd,(struct sockaddr *)&user_addr,&fromlen); 
    if (ns==-1) { 
      perror("\n Erreur accept : \n"); 
      exit(4); 
    } 

    i++; 
    retfork=fork();          
    if(retfork==-1) { 
      perror("\n Erreur fork : \n"); 
      exit(5); 
    } 
      
    // FILS
    if (retfork==0) {  //  on pourrait utiliser write (idem) au lieu  de send 
      retsend=send (ns,buffer,strlen(buffer)+1,0);
      if (retsend==-1) { 
        perror("\n Erreur send : \n"); 
        exit(6); 
      } 
      printf ("\n i= %d retour send = %d \n",i,retsend);
      //mort du fils
      exit(7); 
    }
    
    // PERE
    if(i==NBCLI) exit(8); 
  } 
}
