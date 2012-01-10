/*$Id: provudvdokw.c,v 1.8 2011-02-21 07:35:56 sasa Exp $*/
/*11.11.2008	23.08.2001	Белых А.И. 	provudvdokw.c
Проверка возможности удаления записи в приходном документе
или всего документа
Если вернули 0 -можно удалить
             1 - нельзя
*/
#include        <stdlib.h>
#include	"buhg_g.h"

extern SQL_baza  bd;

int 	provudvdokw(int sklad,//Склад
short dd,short md,short gd,//Дата документа
const char *nomd,//Номер документа
int	kodmat, //Код материалла
int	nomkart,//Номер карточки
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row,row1;
SQLCURSOR       cur;
SQLCURSOR       cur1;
int		kolstr;
struct  tm      *bf;
time_t          tmm;
double kol=0.;
short		d,m,g;
char		naimat[512];
int		vozvrat=0;
class ostatok data;

/*
printw("\nprovudvdok-%d %d.%d.%d %s %d %d\n",
sklad,dd,md,gd,nomd,kodmat,nomkart);
OSTANOV();
*/

time(&tmm);
bf=localtime(&tmm);
d=bf->tm_mday;
m=bf->tm_mon+1;
g=bf->tm_year+1900;

if(kodmat != 0)
 {
  
  ostkarw(1,1,g,31,m,g,sklad,nomkart,&data);
  if((kol=readkolkw(sklad,nomkart,dd,md,gd,nomd,wpredok)) == 0)
    return(0);

  if(data.ostg[3]-kol < -0.000000001)
   {

    memset(naimat,'\0',sizeof(naimat));
    sprintf(strsql,"select naimat from Material where kodm=%d",kodmat);
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      strncpy(naimat,row[0],sizeof(naimat)-1);
    iceb_u_str repl;
    sprintf(strsql,"provudvdokw-%d %s",kodmat,naimat);
    repl.plus(strsql);
    repl.ps_plus(gettext("Склад"));
    repl.plus(":");
    repl.plus(sklad);

    repl.plus(" ");
    repl.plus(gettext("Карточка"));
    repl.plus(":");
    repl.plus(nomkart);
    

    repl.plus(" ");
    repl.plus(gettext("Невозможно удалить приход !"));

    repl.ps_plus(gettext("Документ"));
    repl.plus(":");
    repl.plus(nomd);
    repl.plus(" ");

    sprintf(strsql,"%s %d.%d.%d",gettext("Дата"),d,m,g);
    repl.plus(strsql);
    
    sprintf(strsql,"%s %f",gettext("Отрицательный остаток ! Удаление невозможно !"),data.ostg[3]-kol);
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);

    return(1);
   }
 }
else
 {
  sprintf(strsql,"select nomkar,kodm from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' and tipz=1",
  gd,md,dd,sklad,nomd);
//printw("\n%s\n",strsql);
//OSTANOV();
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

  if(kolstr == 0)
     return(0);  
  
  int nomer_kart=0;
  
  while(cur.read_cursor(&row) != 0)
   {
//    printw("%s %s\n",row[0],row[1]);
    
    nomer_kart=atoi(row[0]);
    ostkarw(1,1,g,31,m,g,sklad,nomer_kart,&data);
    if((kol=readkolkw(sklad,nomer_kart,dd,md,gd,nomd,wpredok)) == 0)
      continue;

    if(data.ostg[3]-kol < -0.000000001)
     {
      vozvrat++;
      memset(naimat,'\0',sizeof(naimat));
      sprintf(strsql,"select naimat from Material where kodm=%s",row[1]);
      if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
        strncpy(naimat,row1[0],sizeof(naimat)-1);

      iceb_u_str repl;
      sprintf(strsql,"provudvdokw-%s %s",row[1],naimat);
      repl.plus(strsql);
      repl.ps_plus(gettext("Склад"));
      repl.plus(":");
      repl.plus(sklad);

      repl.plus(" ");
      repl.plus(gettext("Карточка"));
      repl.plus(":");
      repl.plus(nomkart);
      

      repl.plus(" ");
      repl.plus(gettext("Невозможно удалить приход !"));

      repl.ps_plus(gettext("Документ"));
      repl.plus(":");
      repl.plus(nomd);
      repl.plus(" ");

      sprintf(strsql,"%s %d.%d.%d",gettext("Дата"),d,m,g);
      repl.plus(strsql);
      
      sprintf(strsql,"%s %f",gettext("Отрицательный остаток ! Удаление невозможно !"),data.ostg[3]-kol);
      repl.ps_plus(strsql);
      iceb_menu_soob(&repl,wpredok);

     }
   }

 }
//OSTANOV();
return(vozvrat);
}
