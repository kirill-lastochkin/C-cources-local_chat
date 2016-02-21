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

//�������� ����� ��������� ��� ��������� ��������
#define CONNECT_REQUEST 101L
#define DISCONNECT_REQUEST 102L
#define MESSAGE 103L
#define SHOW_MEMBER 104L
#define HIDE_MEMBER 105L
#define TERMFOR(x) 300L+(long)x
#define MSGFOR(x) (long)x
#define SHFOR(x) (long)x+200L
//��������� ��� ftok
#define FTOK_PATH "/home/"
#define FTOK_INT 'w'
//������������ ����� ������ ���������
#define MSG_MAX_SZ 120

//�������� ���������
struct My_msg
{
    long mtype;
    char msg[MSG_MAX_SZ]; //����� ���������
    pid_t pid;       //���� ��� ���� ����������� ���������
    int registered;  //���� ��� �������� ������ �����������
};

//----------------------��������� �����----------------------------
//��������/�����������/�������� � ������� ���������
int ProcInit(void);
void QueueDelete(int qid);
//������ �������������� ��������� �� �������
void* ProcFunc(void *arg);
void* ConFunc(void *arg);

//��������� ������� ��� ���������� ������
void IntHandler(int arg);
void USR1Handler(int arg);

//---------------------���������� �����----------------------------
//�������
//������������� ������
void InitScreen(void);
//������ � ���� ��������� id ������� � ���������
void PrintMsg(int pos, char *message, long pid);
//������ id � ���� ���������
void PrintClient(int pos, long pid);
//������� ���� 0-1-2: �����-�������-����
void ClearPanel(int panel);
//������� ������
void DeleteScreen(void);
//������ ������
void PrintStr(int pos, int panel, char *str, long int pid);
//������� ��������� �� ������ �����
void GetMessage(char *str);
//������ � �������� ��������� (����������� ��������� �����)
void MessageType(void);
//�������� ������� �� ������
void AddClient(long int pid);
//������� ������� � ������
void RemoveClient(int pos);
//��������� ������� ������� �� ������
int CheckClient(long pid);

//������ �������
//������������� �������
void InitSystem(void);
//�����������/���������� �������
int ClientConnectChat(int type);
//�������� ��������� �� ������
void ClientSendMsg(char* msg);

//������
//����� �������������� ���������
void* MsgProcessor(void *arg);
//����� �������������� ��������� � ����������
void* TrmProcessor(void *arg);
//����� �������������� ��������� � ���������/������������ �������� � ����
void* ShowHideProcessor(void *arg);
#endif
