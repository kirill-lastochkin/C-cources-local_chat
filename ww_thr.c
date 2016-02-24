#include "service.h"

extern int my_number;
int end;

//�����-���������� ��������� � �������
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
        //�������� ��������� - �������� ���
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
        //���� ��������� � ��������-����������� ��� ����
        chk=msgrcv(qid,&msg1,sizeof(msg1),SHFOR(my_number),0);
        if(chk!=-1)
        {
            //��������� ���� �� ������
            chk=CheckClient(msg1.pid);
            //���� ��� - ���������
            if(chk==0)
            {
                AddClient((long int)msg1.pid);
            }
            //����� �������
            else
            {
                RemoveClient(chk);
            }
        }
    }
}
