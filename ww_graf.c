#include "service.h"

extern WINDOW *stdscr;
WINDOW *mpanel[3];
extern int end;

//инициализируем экран как обычно
void InitScreen(void)
{
    end=0;
    initscr();
    cbreak();
    noecho();
    clear();
    curs_set(0);
    start_color();
    init_pair(1,COLOR_YELLOW,COLOR_BLUE);
    init_pair(2,COLOR_RED,COLOR_BLUE);
    //
    mpanel[0]=newwin(getmaxy(stdscr)-3,getmaxx(stdscr)*3/4-1,0,0);
    mpanel[1]=newwin(getmaxy(stdscr),getmaxx(stdscr)/4,0,getmaxx(stdscr)*3/4-1);
    mpanel[2]=newwin(3,getmaxx(stdscr)*3/4-1,getmaxy(stdscr)-3,0);
    //
    wbkgd(mpanel[0],COLOR_PAIR(1));
    wbkgd(mpanel[1],COLOR_PAIR(1));
    wbkgd(mpanel[2],COLOR_PAIR(1));
    //
    wattron(mpanel[0],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[0],0,0);
    wmove(mpanel[0],0,getmaxx(mpanel[0])/2-14);
    wprintw(mpanel[0]," Messages, press F5 to quit ");
    //
    wattron(mpanel[1],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[1],0,0);
    wmove(mpanel[1],0,getmaxx(mpanel[1])/2-4);
    wprintw(mpanel[1]," Members ");
    //
    wattron(mpanel[2],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[2],0,0);
    wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
    wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
    //
    curs_set(1);
    keypad(mpanel[2],TRUE);
    wmove(mpanel[2],1,1);
    wrefresh(mpanel[1]);
    wrefresh(mpanel[0]);
    wrefresh(mpanel[2]);
}

//печать строки в одно из окон
//0 - левое
//1 - правое
void PrintStr(int pos, int panel, char *str, long pid)
{
    wmove(mpanel[panel],pos,1);
    wprintw(mpanel[panel],"%ld:",pid);
    if(panel==0)
    {
        wprintw(mpanel[panel],str);
        box(mpanel[panel],0,0);
        wmove(mpanel[panel],0,getmaxx(mpanel[0])/2-14);
        wprintw(mpanel[panel]," Messages, press F5 to quit ");
    }
    wmove(mpanel[2],1,1);
    wrefresh(mpanel[panel]);
}

//обертка PrintStr, чтоб без путаницы с номерами панелей
void PrintMsg(int pos,char *message,long int pid)
{
    PrintStr(pos,0,message,pid);
}

//обертка PrintStr, чтоб без путаницы с номерами панелей
void PrintClient(int pos,long int pid)
{
    PrintStr(pos,1,NULL,pid);
}

//очистка панели, 0/1/2 по номерам панелей
void ClearPanel(int panel)
{
    wclear(mpanel[panel]);
    wattron(mpanel[panel],COLOR_PAIR(1)| A_BOLD);
    box(mpanel[panel],0,0);
    if(panel==0)
    {
        wmove(mpanel[0],0,getmaxx(mpanel[0])/2-14);
        wprintw(mpanel[0]," Messages, press F5 to quit ");
    }
    if(panel==1)
    {
        wmove(mpanel[panel],0,getmaxx(mpanel[panel])/2-4);
        wprintw(mpanel[panel]," Members ");
    }
    if(panel==2)
    {
        wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
        wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
    }
    wrefresh(mpanel[panel]);
}

//очистка экрана, завершение работы с нкурсес
void DeleteScreen(void)
{
    delwin(mpanel[0]);
    delwin(mpanel[1]);
    delwin(mpanel[2]);
    endwin();
    clear();
}

//получить сообщение из строки ввода
void GetMessage(char *str)
{
    mvwinstr(mpanel[2],1,1,str);
    str[getmaxx(mpanel[2])-2]='\0';
}

//циклический ввод и отправка сообщений
void MessageType(void)
{
    int ch,x,y,i=0;//,j=1;
    char message[getmaxx(mpanel[2])-1];
    while((ch=wgetch(mpanel[2]))!=KEY_F(5)&&end==0)
    {
        wrefresh(mpanel[2]);
        //обработка enter
        if(ch==10)
        {
            GetMessage(message);
            ClearPanel(2);
            wmove(mpanel[2],1,1);
            i=0;
            ClientSendMsg(message);
        }
        else if(ch==KEY_BACKSPACE)
        {
            getyx(mpanel[2],y,x);
            if(x!=1)
            {
                wmove(mpanel[2],y,x-1);
                wdelch(mpanel[2]);
                box(mpanel[2],0,0);
                wmove(mpanel[2],0,getmaxx(mpanel[2])/2-13);
                wprintw(mpanel[2]," Type your message, %ld ",(long)getpid());
                wmove(mpanel[2],1,getmaxx(mpanel[2])-2);
                wprintw(mpanel[2]," ");
                wmove(mpanel[2],y,x-1);
                i--;
            }
        }
        else
        {
            //не даем печатать за границу
            if(i>=getmaxx(mpanel[2])-2)
            {
                continue;
            }
            wprintw(mpanel[2],"%c",ch);
            i++;
        }
    }
}

//добавить клиента на панель
void AddClient(long pid)
{
    int pos;
    char s;
    //ищем первую свободную строку по пробелу
    for(pos=1;pos<200;pos++)
    {
        s=mvwinch(mpanel[1],pos,1);
        if(s==' ')
        {
            break;
        }
    }
    PrintClient(pos,pid);
}

//удаление клиента с указанного номера позиции
void RemoveClient(int pos)
{
    int i,p;
    p=pos;
    char c,st='q';
    //начиная с заданной позиции просто циклически затираем текущую строку нижней
    while(st!=' ')
    {
        wmove(mpanel[1],p,1);
        for(i=1;;i++)
        {
            c=mvwinch(mpanel[1],p+1,i);
            wmove(mpanel[1],p,i);
            wprintw(mpanel[1],"%c",c);
            if(c==' ')
            {
                wprintw(mpanel[1],"    ");
                break;
            }
        }
        p++;
        st=mvwinch(mpanel[1],p,1);
    }
    wrefresh(mpanel[1]);
}

//проверка наличия клиента
//на входе pid клиента, на выходе 0 при отсутствии такого и позиция записи при
//нахождении
int CheckClient(long pid)
{
    int pos,i;
    long cmp;
    char str[10];
    for(pos=1;;pos++)
    {
        for(i=1;i<10;i++)
        {
            str[i-1]=mvwinch(mpanel[1],pos,i);
            if(str[0]==' ')
            {
                return 0; //такого клиента нет
            }
            if(str[i-1]==':')
            {
                str[i-1]='\0';
                break;
            }
        }
        cmp=atol(str);
        if(cmp==pid)
        {
            return pos;
        }

    }

}
