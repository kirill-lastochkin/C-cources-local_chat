#include "service.h"
#define MAX_NUM_OF_CLIENTS 100
pthread_t msgproc,conproc;
int end,NUMBER_OF_CLIENTS;
long clients[MAX_NUM_OF_CLIENTS][2]; //двумерный массив для хранения информации о клиентах и их pid

//создание потоков и подключение к очереди сообщений
//возвращаемое значение - qid очереди
int ProcInit(void)
{
    key_t key;
    int qid=-1,i;
    end=0;
    NUMBER_OF_CLIENTS=0;
    key=ftok(FTOK_PATH,FTOK_INT);
    if(key==-1)
    {
        perror("key\n");
        exit(1);
    }
    for(i=0;i<MAX_NUM_OF_CLIENTS;i++)
    {
        clients[i][0]=0;
        clients[i][1]=0;
    }
    //сигналы нужны для прерывания работы
    signal(SIGINT,IntHandler);
    signal(SIGUSR1,USR1Handler);
    //создаем очередь
    qid=msgget(key,IPC_CREAT|0666|IPC_EXCL);
    //если же такая же есть, то закрываем ее и пересоздаем заново
    if(qid==-1)
    {
        qid=msgget(key,0);
        msgctl(qid,IPC_RMID,NULL);
        qid=msgget(key,IPC_CREAT|0666|IPC_EXCL);
    }
    qid=msgget(key,0);
    //
    if(qid==-1)
    {
        perror("qid\n");
        exit(1);
    }
    //запускаем процессы обработки сообщений
    pthread_create(&msgproc,NULL,ProcFunc,&key);
    pthread_create(&conproc,NULL,ConFunc,&key);
    pthread_join(conproc,NULL);
    //pthread_join(msgproc,NULL);
    return qid;
}

//удаление очереди по qid
void QueueDelete(int qid)
{
    int chk=-2;
    struct My_msg msg1;
    //вытаскиваем оставшиеся сообщения
    while(chk!=-1)
    chk=msgrcv(qid,&msg1,sizeof(msg1),0,0);
    msgctl(qid,IPC_RMID,NULL);
    printf("queue deleted\n");
}

//поток-обработчик сообщений с текстом
void* ProcFunc(void *arg)
{
    struct My_msg msg1;
    int chk;
    key_t key;
    int qid;
    key=*((key_t*)arg);
    msgget(key,0);
    qid=msgget(key,0);
    while(end==0)
    {
        //ждем сообщения заданного типа
        chk=msgrcv(qid,&msg1,sizeof(msg1),MESSAGE,0);
        if(chk==-1)
        {
            perror("receive error\n");
        }
        else
        {
            //отправляем рассылку
            for(chk=1;chk<=MAX_NUM_OF_CLIENTS;chk++)
            {
                //ставим в качестве типа сообщения номер того, комy оно предназначается
                if(clients[chk-1][0]!=0)
                {
                    msg1.mtype=MSGFOR(chk);
                    msgsnd(qid,&msg1,sizeof(msg1),0);
                }
            }
            printf("message was sent to %d clients\n",NUMBER_OF_CLIENTS);
        }
    }
    printf("msgproc succeed\n");
    return NULL;
}

//поток-обработчик служебных сообщений
void* ConFunc(void *arg)
{
    struct My_msg msg1;
    pid_t pid_save;
    int chk,i;
    key_t key;
    int qid;
    key=*((key_t*)arg);
    msgget(key,0);
    qid=msgget(key,0);
    while(end==0)
    {
        //ждем сообщения типа запрос на соединение-отключение
        chk=msgrcv(qid,&msg1,sizeof(msg1),CONNECT_REQUEST,0);
        if(chk==-1)
        {
            perror("receive error\n");
        }
        else
        {
            //если не зареган, то регаем, рассылаем всем сообщения, что
            //нужно отобразить нового участника
            //в поле регистрации пишем порядковый номер нового участника начиная с 1
            //получатель запишет свой номер по получении
            //понять что нужно записать свой номер - сравнение своего пида и в сообщении
            if(msg1.registered==0)
            {
                //сначала рассылаем подключенным клиентам
                for(i=1;i<=MAX_NUM_OF_CLIENTS;i++)
                {
                    if(clients[i-1][0]!=0)
                    {
                        msg1.mtype=SHFOR(i); //рассылка производится каждому по его номеру
                        msgsnd(qid,&msg1,sizeof(msg1),0);
                    }
                }
                //увеличиваем число активных клиентов
                NUMBER_OF_CLIENTS++;
                msg1.mtype=SHOW_MEMBER;
                pid_save=msg1.pid;
                //ищем минимальный свободный номер
                //занимаем ячейку, его номер возвращаем клиенту через поле registered
                for(i=1;i<=MAX_NUM_OF_CLIENTS;i++)
                {
                    //ищем первый свободный номер
                    if(clients[i-1][0]==0)
                    {
                        if(msg1.registered==0)
                        {
                            clients[i-1][0]=1;
                            clients[i-1][1]=(long)pid_save;
                            msg1.registered=i;
                        }
                    }
                    //остальных зарегистрированных тоже высылаем клиенту, чтобы отобразил
                    else
                    {
                        msg1.pid=(pid_t)clients[i-1][1];
                        msgsnd(qid,&msg1,sizeof(msg1),0);
                    }
                }
                //последним шлем его собственный pid
                msg1.pid=pid_save;
                msgsnd(qid,&msg1,sizeof(msg1),0);
                printf("client %d registered with number %d\n",NUMBER_OF_CLIENTS,msg1.registered);
            }
            //если зареган, значит, хочет отключиться
            //сообщаем всем, что нужно его убрать из списка участников
            else
            {
                clients[msg1.registered-1][0]=0;
                clients[msg1.registered-1][1]=0;
                for(i=1;i<=MAX_NUM_OF_CLIENTS;i++)
                {
                    if(clients[i-1][0]!=0)
                    {
                        msg1.mtype=SHFOR(i);
                        msgsnd(qid,&msg1,sizeof(msg1),0);
                    }

                }
                msg1.mtype=HIDE_MEMBER;
                msgsnd(qid,&msg1,sizeof(msg1),0);
                NUMBER_OF_CLIENTS--;
                printf("client with number %d disconnected\n",msg1.registered);
                msg1.registered=0;
            }
        }
    }
    //если работа прервана, отправляем сигнал окончания всем клиентам
    for(chk=0;chk<MAX_NUM_OF_CLIENTS;chk++)
    {
        if(clients[chk][0]!=0)
        {
            msg1.mtype=TERMFOR(chk+1);
            msgsnd(qid,&msg1,sizeof(msg1),0);
        }
    }
    printf("conproc succeed\n");
    return NULL;
}

//обработчик sigint отправляет сигналы потокам
//потоки находятся в режиме ожидания сообщения, которое прерывается
//флаг end прекращает выполенение цикла, что приводит к завершению всей работы
void IntHandler(int arg)
{
    end=1;
    arg++;
    pthread_kill(conproc,SIGUSR1);
    pthread_kill(msgproc,SIGUSR1);
}

//прерывание просто чтобы прервать msgrcv
void USR1Handler(int arg)
{
    arg++;
}
