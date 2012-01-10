/* $Id: podtdok1w.c,v 1.8 2011-02-21 07:35:55 sasa Exp $ */
/*22.09.2004     30.5.1995       Белых А.И.      podtdok1w.c
Подтверждение всех записей в документе сразу
*/
#include        <stdlib.h>
#include        <time.h>
#include        <math.h>
#include	"buhg_g.h"

extern SQL_baza bd;

void podtdok1w(short dd,short md,short gd,int skl,const char *nomdok,
int tipz,short d1,short m1,short g1,
int metkaeho, //0-выдавать на экран 1-работать молча
GtkWidget *wpredok)
{
double          kol1,kol2;
short           dp,mp,gp;
SQL_str         row,row1;
int		kolstr;
char		strsql[512];
char		naimat[512];
int		nk;
double		cena;
struct  tm      *bf;
time_t          tmm;


time(&tmm);
bf=localtime(&tmm);

short mmm=bf->tm_mon+1;
short ggg=bf->tm_year+1900;


sprintf(strsql,"select * from Dokummat1 where datd='%d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
SQLCURSOR cur1;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,"podtdok1-Ошибка создания курсора",strsql,wpredok);
  return;
 }

if(kolstr == 0 )
 {
  if(metkaeho == 0)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найдено ни одной записи !"));
    iceb_menu_soob(&repl,wpredok);
   }
  return;
 }

class ostatok ost;

while(cur.read_cursor(&row) != 0)
 {
  nk=atoi(row[3]);
  if(nk == 0)
   {
//    if(metkaeho == 0)
//      printw(gettext("не привязана к карточке\n"));
    continue;
   }
  kol1=atof(row[5]);
  cena=atof(row[6]);
/*******************************
  if(metkaeho == 0)
   {
    //Узнаем наименование материалла
    sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
    if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
     {
        beep();
        printw("%s %s !\n",gettext("Не найден код материалла"),row[4]);
        OSTANOV();
     }
    strncpy(naimat,row1[0],sizeof(naimat)-1);

    printw("%-4s %-4s %-20.20s %10.10g - ",
    row[3],row[4],naimat,kol1);
    refresh();
   }
*************************/


  if(tipz == 2)
   {
    prdppw(skl,nk,&dp,&mp,&gp,wpredok);

//    if(SRAV1(g1,m1,d1,gp,mp,dp) > 0)
    if(iceb_u_sravmydat(d1,m1,g1,dp,mp,gp) < 0)
     {
      if(metkaeho == 0)
       {
        memset(naimat,'\0',sizeof(naimat));
        //*Узнаем наименование материалла
        sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
        if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
          strncpy(naimat,row1[0],sizeof(naimat)-1);

        iceb_u_str repl;
        repl.plus(row[4]);
        repl.plus(" ");
        repl.plus(naimat);
        
        sprintf(strsql,gettext("\nДата подтверждения (%d.%d.%dг.) меньше чем дата первого прихода (%d.%d.%dг.)\n\
Такой расход не может быть ! Не подтверждено.\n"),d1,m1,g1,dp,mp,gp);
        repl.ps_plus(strsql);
        iceb_menu_soob(&repl,wpredok);
       }
      continue;
     }
   }

  /*Читаем в карточке количество реально выданное-полученное*/

  kol2=readkolkw(skl,nk,dd,md,gd,nomdok,wpredok);

  
  ostkarw(1,1,ggg,31,mmm,ggg,skl,nk,&ost);
  if(kol1 > kol2)
   {
     if(tipz == 2 && kol1-kol2 - ost.ostg[3] > 0.000000001)
      {
       if(metkaeho == 0)
        {
         //   printw(gettext("неподтверждена (количество)\n"));
         memset(naimat,'\0',sizeof(naimat));
         //*Узнаем наименование материалла
         sprintf(strsql,"select naimat from Material where kodm=%s",row[4]);
         if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
           strncpy(naimat,row1[0],sizeof(naimat)-1);

         iceb_u_str repl;
         repl.plus(row[4]);
         repl.plus(" ");
         repl.plus(naimat);
         repl.ps_plus(gettext("Отрицательный остаток ! Подтверждение невозмажно !"));
         iceb_menu_soob(&repl,wpredok);

        }
       continue;
      } 
//     if(metkaeho == 0)
//       printw(gettext("подтверждена\n"));
     zapvkrw(d1,m1,g1,nomdok,skl,nk,dd,md,gd,tipz,kol1-kol2,cena,wpredok);
   }
//  else
//   if(metkaeho == 0)
//     printw(gettext("подтверждена раньше\n"));

 }


}
