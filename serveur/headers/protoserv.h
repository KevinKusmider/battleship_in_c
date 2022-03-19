#ifndef __PROTOSERV_H__
#define __PROTOSERV_H__

    typedef struct RESPONSE
    {
        char *fullResponse;
        char *type;
        char *content;
    } RESPONSE;
    

    int send_answer(int ns, char * type, char * content);
    int listen_answer(int ns, RESPONSE &response);
    void clean_stdin();

#endif