#ifndef __PROTOCOMCLI_H__
#define __PROTOCOMCLI_H__

    typedef struct GAME 
    {
        char plate[500];
        int width;
        int height;
    } GAME;

    typedef struct RESPONSE
    {
        char fullResponse[100];
        char type[100];
        char *content;
    } RESPONSE;

    int send_asked_str(int ns, char *question, char *type);
    void show_response(RESPONSE *response);
    int reset_response(RESPONSE *response);
    int send_response(int ns, char * type, char * content);
    int listen_response(int ns, RESPONSE *response);
    void clean_stdin();

#endif