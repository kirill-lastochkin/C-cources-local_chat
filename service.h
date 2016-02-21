#ifndef _SERVICE_H
#define _SERVICE_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <sys/syscall.h>
#include <errno.h>

//описание типов сообщений для системных запросов
#define CONNECT_REQUEST 101L
#define DISCONNECT_REQUEST 102L
#define MESSAGE 103L
#define SHOW_MEMBER 104L
#define HIDE_MEMBER 105L
#define TERMFOR(x) 300L+(long)x
#define MSGFOR(x) (long)x
#define SHFOR(x) (long)x+200L
//параметры для ftok
#define FTOK_PATH "/home/"
#define FTOK_INT 'w'
//максимальная длина текста сообщения
#define MSG_MAX_SZ 120

//описание сообщения
struct My_msg
{
    long mtype;
    char msg[MSG_MAX_SZ]; //текст сообщения
    pid_t pid;       //поле для пида отправителя сообщения
    int registered;  //поле для передачи номера регистрации
};

//----------------------серверная часть----------------------------
//создание/подключение/удаление к очереди сообщений
int ProcInit(void);
void QueueDelete(int qid);
//потоки обрабатывающие сообщения из очереди
void* ProcFunc(void *arg);
void* ConFunc(void *arg);

//обработка сигнало для завершения работы
void IntHandler(int arg);
void USR1Handler(int arg);

//---------------------клиентская часть----------------------------
//графика
//инициализация экрана
void InitScreen(void);
//печать в окно сообщений id клиента и сообщение
void PrintMsg(int pos, char *message, long pid);
//печать id в окно сообщений
void PrintClient(int pos, long pid);
//очистка окна 0-1-2: сообщ-клиенты-ввод
void ClearPanel(int panel);
//очистка экрана
void DeleteScreen(void);
//печать строки
void PrintStr(int pos, int panel, char *str, long int pid);
//достать сообщение из строки ввода
void GetMessage(char *str);
//печать и отправка сообщений (циклическая обработка ввода)
void MessageType(void);
//добавить клиента на панель
void AddClient(long int pid);
//удалить клиента с панели
void RemoveClient(int pos);
//проверить наличие клиента на панели
int CheckClient(long pid);

//работа клиента
//инициализация системы
void InitSystem(void);
//полключение/отключение сервера
int ClientConnectChat(int type);
//отправка сообщения на сервер
void ClientSendMsg(char* msg);

//потоки
//поток обрабатывающий сообщения
void* MsgProcessor(void *arg);
//поток обрабатывающий сообщение о завершении
void* TrmProcessor(void *arg);
//поток обрабатывающий сообщения о появлении/исчезновении клиентов в чате
void* ShowHideProcessor(void *arg);
#endif
