/*$Id: iceb_readkey.c,v 1.9 2011-02-21 07:36:07 sasa Exp $*/
/*19.03.2010	12.08.2003	Белых А.И.	iceb_readkey.c
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include <unistd.h>
#include         "iceb_libbuh.h"

int iceb_vb(const char *host,const char *parol,char **imabaz,GtkWidget *wpredok);

extern int optind, opterr, optopt;
extern char *optarg;

int iceb_t_vb(const char *host,const char *parol,char **imabaz,GtkWidget *wpredok);

void	iceb_readkey(int argc,char *argv[],
char **parol, //Паpоль
char **imabaz, //Имя базы
char **host, //Хост
int *regim) //Режим pаботы пpогpаммы
{
int i=0;

/* Просмотр ключей    */
/*
b - имя базы
h - хост
p - пароль
r - режим работы программы, для разных подсистем по разному
*/
*regim=0;
while ((i = getopt(argc, argv, "b:h:p:r:")) != EOF) 
 {
  switch (i) 
   {
    case 'b': /* Имя базы*/
      (*imabaz)=new char[strlen(optarg)+1];
      strcpy(*imabaz,optarg);
      break;

    case 'h': /* Хост*/
      (*host)=new char[strlen(optarg)+1];
      strcpy(*host,optarg);
      break;

    case 'p': /* Пароль*/
      (*parol)=new char[strlen(optarg)+1];
      strcpy(*parol,optarg);
      break;

    case 'r': /* Режим работы программы*/
      *regim=atoi(optarg);
      break;

   }
 }

if(*host == NULL)
 {
  (*host)=new char[strlen("localhost")+1];
  strcpy(*host,"localhost");
 }

if(*parol == NULL)
 {
  class iceb_u_str parstr("");
  if((*parol = getenv("PAROLICEB")) == NULL)
   {
    if(iceb_parol(&parstr,NULL) != 0)
     exit(0);

    if(parstr.getdlinna() > 1)
     {
      (*parol)=new char[parstr.getdlinna()+1];
      strcpy(*parol,parstr.ravno());
     }
    else 
     {
      (*parol)=new char[strlen("aaa")+1];
      strcpy(*parol,"aaa");
     }    
   }
 }

if(*imabaz == NULL)
 {
  if(iceb_vb(*host,*parol,&*imabaz,NULL) == 0)
   return;
  else
   iceb_exit(0);
 }

/*******
if(*imabaz == NULL)
  iceb_vparol(parol,imabaz);
********/

}
