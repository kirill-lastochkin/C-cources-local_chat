#include "service.h"

extern int my_number;
int end;

//поток-обработчик сообщений с текстом
void* MsgProcessor(void *arg)
{
    key_t key;
    struct My_msg msg1;
    int qid,chk,pos=1;
    key=ftok(FTOK_PATH,FTOK_INT);
    qid=msgget(key,0);
    if(qid==-1)
    {
        perror("no queue\n");
        return NULL;
    }
    while(end==0)
    {
        //получаем сообщение - печатаем его
        chk=msgrcv(qid,&msg1,sizeof(msg1),MSGFOR(my_number),0);
        if(chk!=-1)
        {
            PrintMsg(pos++,msg1.msg,(long)msg1.pid);
        }
    }
}


void* ShowHideProcessor(void *arg)
{
    key_t key;
    struct My_msg msg1;
    int qid,chk;
    key=ftok(FTOK_PATH,FTOK_INT);
    qid=msgget(key,0);
    if(qid==-1)
    {
        perror("no queue\n");
        return NULL;
    }
    while(end==0)
    {
        //ждем сообщения о коннекте-дисконнекте для себя
        chk=msgrcv(qid,&msg1,sizeof(msg1),SHFOR(my_number),0);
        if(chk!=-1)
        {
            //проверяем есть ли клиент
            chk=CheckClient(msg1.pid);
            //если нет - добавляем
            if(chk==0)
            {
                AddClient((long int)msg1.pid);
            }
            //иначе удаляем
            else
            {
                RemoveClient(chk);
            }
        }
    }
}
