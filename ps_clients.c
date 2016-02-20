#include "service.h"

pthread_t msgwait,trmwait,swait,hwait;
int my_number,member_list_pos;
//extern int end;
//потом убрать
//extern WINDOW *mpanel[3];

void InitSystem(void)
{
    //int arg1,arg2;
   // arg1=0;arg2=1;
    member_list_pos=1;
    my_number=ClientConnectChat(1);
    if(my_number==-1)
    {
        perror("glob error\n");
        DeleteScreen();
        exit(1);
    }
    pthread_create(&msgwait,NULL,MsgProcessor,NULL);
    pthread_create(&trmwait,NULL,TrmProcessor,NULL);
    pthread_create(&swait,NULL,ShowHideProcessor,NULL);
   // pthread_create(&hwait,NULL,ShowHideProcessor,&arg2);
}



//при регистрации указывать 1, при отклбчении 0
int ClientConnectChat(int type)
{
    key_t key;
    pid_t pid;
    pid=getpid();
    struct My_msg msg1;
    int qid,chk;
    key=ftok(FTOK_PATH,FTOK_INT);
    qid=msgget(key,0);
    if(qid==-1)
    {
        perror("no queue\n");
        clear();
        return -1;
    }
    msg1.mtype=CONNECT_REQUEST;
    msg1.pid=pid;
    if(type)
    {
        msg1.registered=0;
    }
    else
    {
        msg1.registered=my_number;
    }
    chk=msgsnd(qid,&msg1,sizeof(msg1),0);
    if(chk==-1)
    {
        perror("client failed to connect chat:send\n");
        return -1;
    }
    if(type)
    {
        chk=msgrcv(qid,&msg1,sizeof(msg1),SHOW_MEMBER,0);
    }
    else
    {
        chk=msgrcv(qid,&msg1,sizeof(msg1),HIDE_MEMBER,0);
    }
    if(chk==-1)
    {
        perror("client failed to connect chat:receive\n");
        return -1;
    }
    if(type)
    {
        //на случай если кто-то проскочит раньше в очереди, тогда просто печатаем их имена
        while(pid!=msg1.pid)
        {
            AddClient(msg1.pid);
            chk=msgrcv(qid,&msg1,sizeof(msg1),SHOW_MEMBER,0);
            if(chk==-1)
            {
                perror("client failed to connect chat:receive another\n");
                return -1;
            }
        }
        AddClient(msg1.pid);
    }
//    else
//    {
//        while(pid!=msg1.pid)
//        {
//            PrintClient(member_list_pos++,"somename diconnected");
//            chk=msgrcv(qid,&msg1,sizeof(msg1),HIDE_MEMBER,0);
//            if(chk==-1)
//            {
//                perror("client failed to connect chat:receive another\n");
//                return -1;
//            }
//        }
//        PrintClient(member_list_pos++,"i'm disconnected");
//    }
    return msg1.registered;
}



void ClientSendMsg(char* msg)
{
    key_t key;
    pid_t pid;
    pid=getpid();
    struct My_msg msg1;
    int qid,chk;
    key=ftok(FTOK_PATH,FTOK_INT);
    qid=msgget(key,0);
    if(qid==-1)
    {
        perror("no queue\n");
        return;
    }
    msg1.mtype=MESSAGE;
    msg1.pid=pid;
    strcpy(msg1.msg,msg);
    chk=msgsnd(qid,&msg1,sizeof(msg1),0);
    if(chk==-1)
    {
        perror("message send err\n");
        return;
    }
}
