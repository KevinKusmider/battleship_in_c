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
#define PORT 3550 
#define ADDRESS "127.0.0.1" 

int create_game(char *plate, int height, int width);
int check_target(GAME *game, char *target);
int get_plate_indice(GAME *game, char *target);

//============================================================== 
//  SERVEUR MULTICLIENT (CHAQUE CLIENT EST DS UNE MACHINE      = 
//           DISTINCTE DU SERVEUR)                             = 
//  L'adresse IP du serveur est dans ADRESSE le port dans PORT = 
//============================================================== 
int main() 
{     
  int sd, ns, fromlen, i, retfork, retsend;

  int shmID, CLE = 2500;
  GAME *game = NULL;

  shmID = shmget((key_t)CLE, sizeof(GAME), IPC_CREAT|0700);
  game = shmat(shmID, NULL, 0);
  game->ready = 0;
  game->started = 0;

  game->playerOne.id = '\0';
  strcpy(game->playerOne.name, "\0");
  strcpy(game->playerOne.pass, "\0");
  game->playerOne.isLogged = '\0';
  game->playerTwo.id = '\0';
  strcpy(game->playerTwo.name, "\0");
  strcpy(game->playerTwo.pass, "\0");
  game->playerTwo.isLogged = '\0';


  struct sockaddr_in my_addr, user_addr ; 
     
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
    perror("\n Erreur listen : \n"); 
    exit(3); 
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

      int user = 0; // 0 : Not connected, 1 : Admin, 2+ : client
      int connected = 1;
      int boats = 0;

      RESPONSE *response;
      if(NULL == (response = malloc(sizeof(RESPONSE)))) {exit(6);}

      while (connected) {
        // Send auth request if not logged in
        if(!user) send_response(ns, "login", "");

        listen_response(ns, response);
        //show_response(response);

        // Authentication
        if(!strcmp(response->type, "login")) {
          if(!strcasecmp(response->content, "Admin")) {
            user = 1;
            send_response(ns, "show_admin_menu", "");

          } else {
            char name[100];
            strcpy(name, response->content);
            send_response(ns, "password", "");
            listen_response(ns, response);
            if(!strcmp(game->playerOne.name, name) && !strcmp(game->playerOne.pass, response->content)) {
              user = game->playerOne.id;
              game->playerOne.isLogged = 1;
              send_response(ns, "logged", game->playerOne.name);
            }
            if(!strcmp(game->playerTwo.name, name) && !strcmp(game->playerTwo.pass, response->content)) {
              user = game->playerTwo.id;
              game->playerTwo.isLogged = 1;
              send_response(ns, "logged", game->playerTwo.name);
            }
        

            if(user > 1) {
              while (!game->started) {
                sleep(2);
                send_response(ns, "show", "\nAttente...");
                sleep(1);
              }

              int targetIndex = 0; 

              while(game->started) {
                if(game->currentPlayer == user) {
                  send_response(ns, "ask_target", game->plateC);
                  listen_response(ns, response);
                  if(check_target(game, response->content) == 1) {
                    targetIndex = get_plate_indice(game, response->content);
                    if(targetIndex > 0 && game->plate[targetIndex] == 'H') {
                      game->plate[targetIndex] = 'X';
                      game->plateC[targetIndex] = 'X';
                      if(game->currentPlayer == 2) {
                        game->playerOne.score = game->playerOne.score+1;
                      } else {
                        game->playerTwo.score = game->playerTwo.score+1;
                      }
                    } else {
                      game->plate[targetIndex] = 'O';
                      game->plateC[targetIndex] = 'O';
                    }
                    game->currentPlayer = game->currentPlayer == 2 ? 3 : 2;
                    send_response(ns, "show", game->plateC);
                  }
                }
              } // END OF GAME
            }
          }
        }

        // Admin actions
        if(user == 1) {
          if(!strcmp(response->type, "create_user")) {
            printf("\nCréation d'un utilisateur");
            char * strToken = strtok ( response->content, ",");

            int id;
            char name[100];
            char pass[100];
            int id2;
            char name2[100];
            char pass2[100];
            id = strToken ? atoi(strToken) : 0;
            strToken = strtok ( NULL, ",");
            strcpy(name, strToken);
            strToken = strtok ( NULL, ",");
            strcpy(pass, strToken);
            strToken = strtok ( NULL, ",");
            id2 = strToken ? atoi(strToken) : 0;
            strToken = strtok ( NULL, ",");
            strcpy(name2, strToken);
            strToken = strtok ( NULL, ",");
            strcpy(pass2, strToken);

            game->playerOne.id = id;
            strcpy(game->playerOne.name,name);
            strcpy(game->playerOne.pass,pass);

            game->playerTwo.id = id2;
            strcpy(game->playerTwo.name,name2);
            strcpy(game->playerTwo.pass,pass2);

            send_response(ns, "show_admin_menu", "");

          } 

          if(!strcmp(response->type, "list_users")) {
            printf("\nAffichage des utilisateurs");
            send_response(ns, "show", game->playerOne.name);
            sleep(1);
            send_response(ns, "show", game->playerTwo.name);
            sleep(1);
            send_response(ns, "show_admin_menu", "");
          } 

          if(!strcmp(response->type, "create_game")) {
            char * strToken = strtok ( response->content, ",");

            int row, col;
            row = strToken ? atoi(strToken) : 0;
            strToken = strtok ( NULL, ",");
            col = strToken ? atoi(strToken) : 0;
            game->width = col;
            game->height = row;
            game->currentPlayer = 2;
            boats = 0;
            create_game(game->plate, row, col);
            create_game(game->plateC, row, col);
            send_response(ns, "ask_boat", game->plate);
          }

          if(!strcmp(response->type, "add_boat")) {
              int index;
              printf("\n%s\n", response->content);
              if(strcmp(response->content, "0")) {
                if((index = get_plate_indice(game, response->content)) != -1) {
                  if(game->plate[index] == 'H') {
                    printf("\nImpossible\n");
                  } else {
                    game->plate[index] = 'H';
                    boats++;
                  }
                }
              } else { 
                boats++; 
              }

            if(boats > 2) {
              game->ready = 1;
              send_response(ns, "show_admin_menu", game->plate);
            } else {
              send_response(ns, "ask_boat", game->plate);
            }
          }

          if(!strcmp(response->type, "start_game")) {
            int ready = 1;
            if(game->playerOne.isLogged != 1 || game->playerTwo.isLogged != 1) ready = 0;
            if(game->ready != 1) ready = 0;

            if(ready) game->started = 1;
            else send_response(ns, "show_admin_menu", "\nLes joueurs ne sont pas connectés / Le plateau n'est pas créé\n");
          }

        } // End admin actions
      }


      
      printf("end\n");
      //mort du fils
      exit(7); 
    }
    
    // PERE
    if(i > NBCLI) exit(8); 
  } 
}

int create_game(char *plate, int height, int width) {
  int row, col, i;

	int index = 0;

  // Reset plate
  for(i=0 ; i<499; i++) {
    plate[i] = '\0';
  }
  
	for(row=0 ; row<(height+1) ; row++) {
		for(col=0 ; col<(width*2)+1; col++) {
			if(!row) { // first row
				if(!col) {
					plate[index++] = '-';
				} else {
					if(col%2) {
						plate[index++] = ' ';
					} else {
						plate[index++] = 47 + col/2;
					}
				}
			} else {
				if(!col) {
					plate[index++] = 64+row;
				} else {
					if(col%2) {
						plate[index++] = ' ';
					} else {
						plate[index++] = '~';
					}
				}
			}
		}
		plate[index++] = '\n';
	}
  return 1;
}

int check_target(GAME *game, char *target) {
  printf("\ntarget : %d\n", target[0] - 65);
  if (target[0] == '0') {
    //printf("\ncheck 0\n");
    return 0;
  }

  if(((target[0] - 65) >= game->height) || ((target[0] - 65) < 0)) {
    //printf("\ncheck 0\n");
    return 0;
  }

  if(((target[1] - 48) >= game->width) || ((target[1] - 48) < 0)) {
    //printf("\ncheck 0\n");
    return 0;
  }  

  return 1;
}

int get_plate_indice(GAME *game, char *target) {
  if(!check_target(game, target)) return -1;

  int row, col, index;
  row = target[0] - 65;
  col = target[1] - 48;

  printf("\ncol : %d\nrow : %d\nwidth : %d\n", col, row, game->width);
  index = (2*(col+1)) + ((row+1)*(game->width+1)*2);

  return index;
}

