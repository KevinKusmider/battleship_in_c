#ifndef __PROTOCOMCLI_H__
#define __PROTOCOMCLI_H__

    void sendLogin(int ns);
    int send_answer(int ns, char * type, char * content);
    int listen_answer(int ns);

#endif