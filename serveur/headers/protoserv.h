#ifndef __PROTOSERV_H__
#define __PROTOSERV_H__

    typedef struct PLAYER {
        int id;
        char name[100];
        char pass[100];
        int score;
        int isLogged;
    } PLAYER;

    typedef struct GAME 
    {
        char plate[500];
        char plateC[500];
        int width;
        int height;
        int currentPlayer;
        int ready;
        int started;
        PLAYER playerOne;
        PLAYER playerTwo;
    } GAME;

    typedef struct RESPONSE
    {
        char fullResponse[1000];
        char type[100];
        char *content;
    } RESPONSE;
    
    void show_response(RESPONSE *response);
    int reset_response(RESPONSE *response);
    int send_response(int ns, char * type, char * content);
    int listen_response(int ns, RESPONSE *response);
    void clean_stdin();

#endif