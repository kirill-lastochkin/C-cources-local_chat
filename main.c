/*
 * ��� ���������� ������� ��������� ���� -D server , ����� ww_que.c main.c
 * ��� ���������� ������� ��������� ���� -D chat , ����� main.c ww_graf.c ww_thr.c ps_clients.c
 *
 * */

#include "service.h"

int main(void)
{
#ifdef chat
    InitScreen();
    InitSystem();
    MessageType();
    ClientConnectChat(0);
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

