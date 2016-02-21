/*
 * при компил€ции сервера указывать ключи -D server -lpthread, файлы ww_que.c main.c
 * при компил€ции клиента указывать ключи -D chat -lpthread -lncurses, файлы main.c ww_graf.c ww_thr.c ps_clients.c
 *
 * */

#include "service.h"

extern int end;
extern pthread_t msgwait,trmwait,swait;

int main(void)
{
#ifdef chat
    InitScreen();
    InitSystem();
    MessageType();
    if(!end)
    {
        ClientConnectChat(0);
    }
    DeleteScreen();
    printf("chat successfully ended\n");
#endif
#ifdef server
    int qid;
    qid=ProcInit();
    QueueDelete(qid);
    printf("end server\n");
#endif

    return 0;
}

