#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <unistd.h>
#include <arpa/inet.h> 
#include <sys/shm.h>

#include "../headers/protoserv.h"

#define NBCLI 3 
#define MAXPLAYERS 2
#define PORT 3550 
#define ADDRESS "127.0.0.1" 

int max_player_id(PLAYER *players);

//============================================================== 
//  SERVEUR MULTICLIENT (CHAQUE CLIENT EST DS UNE MACHINE      = 
//           DISTINCTE DU SERVEUR)                             = 
//  L'adresse IP du serveur est dans ADRESSE le port dans PORT = 
//============================================================== 
int main() 
{     
  int sd, ns, fromlen, i, retfork, retsend;

  int shmID, shmID_players, CLE = 1212;
  GAME *game = NULL;
  PLAYER *players = NULL;

  shmID = shmget((key_t)CLE, sizeof(GAME), IPC_CREAT|0700);
  game = shmat(shmID, NULL, 0);

  shmID_players = shmget((key_t)1700, MAXPLAYERS*sizeof(PLAYER), IPC_CREAT|0700);
  players = shmat(shmID_players, NULL, 0);

  struct sockaddr_in my_addr, user_addr ; 
 
  char *buffer= "a\0bc";      
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
    ns = accept(sd,(struct sockaddr *)&user_addr,&fromlen); 
    if (ns == -1) { 
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
      int i = 0;
      int user = 0; // 0 : Not connected, 1 : Client, 2 : Admin
      int playing = 1;
      RESPONSE *response;
      if(NULL == (response = malloc(sizeof(RESPONSE)))) {
        exit(6);
      }

      while (playing) {
        // Send auth request if not logged in
        if(!user) send_response(ns, "login", "");

        listen_response(ns, response);
        show_response(response);

        // Authentication
        if(!strcmp(response->type, "login")) {
          if(!strcmp(response->content, "Admin")) {
            user = 1;
            send_response(ns, "show_admin_menu", "");
            players[i].id = 4;
            strcpy(players[i].name, "test");
            strcpy(players[i].pass, "test");
          } else {
            char name[100];
            strcpy(name, response->content);
            send_response(ns, "password", "");
            listen_response(ns, response);
            for(i=0 ; i<MAXPLAYERS ; i++) {
              if(!strcmp(players[i].name, name) && !strcmp(players[i].pass, response->content)) {
                printf("\nmax_player_id %d\n", max_player_id(players));
                send_response(ns, "logged", players[i].name);
              }
            }
          }
        }

        // Admin actions
        if(user == 1) {
          if(!strcmp(response->type, "create_user")) {
            printf("\nCréation d'un utilisateur");
          } 

          if(!strcmp(response->type, "list_users")) {
            printf("\nAffichage des utilisateurs");
          } 
        }
      }
      
      printf("end\n");
      //mort du fils
      exit(7); 
    }
    
    // PERE
    if(i==NBCLI) exit(8); 
  } 
}


int max_player_id(PLAYER *players) {
  int i = 0, max = 2;

  for(i=0 ; i<MAXPLAYERS; i++) {
    if(players[i].id > max) {
      max = players[i].id;
    }
  }

  return max;
}