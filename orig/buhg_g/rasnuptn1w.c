/* $Id: rasnuptn1w.c,v 1.7 2011-02-21 07:35:56 sasa Exp $ */
/*15.11.2009	22.10.1999	Белых А.И.	rasnuptn1w.c
Распечатка всех начислений и удержаний за определенный месяц
без учета сальдо
*/
#include <stdlib.h>
#include        <errno.h>
#include        <time.h>
#include        "buhg_g.h"

extern short	*obud; /*Обязательные удержания*/
extern SQL_baza bd;

void rasnuptn1w(short mn,short gn,short dk,short mk,short gk,
int tabn,char fio[],FILE *ff,GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		prn;
short		d,m,g;
short		knah;
double		sym,itog,inah,iuder,iobud;
char		priz[2];
char		nai[512];
int		i;

if(obud == NULL)
 {
  sprintf(strsql,"ranuptn1-%s",gettext("Не введены \"Обязательные удержания\" !"));
  iceb_menu_soob(strsql,wpredok);
  return;
 }

fprintf(ff,"%s %s %d.%d%s\n%d %s\n",
gettext("Начисления и удержания без учета сальдо"),
gettext("За"),mn,gn,
gettext("г."),tabn,fio);

if(dk != 0)
 fprintf(ff,"%s %d.%d.%d%s\n",gettext("По"),dk,mk,gk,
 gettext("г."));

fprintf(ff,"%s\n",gettext("Коды обязательных платежей:"));

for(i=1; i < obud[0]+1 ; i++)
 fprintf(ff,"%d ",obud[i]);

if(dk == 0)
 sprintf(strsql,"select * from Zarp where tabn=%d and \
mesn=%d and godn=%d and suma <> 0. order by datz,prn,knah asc",
 tabn,mn,gn);
else
 sprintf(strsql,"select * from Zarp where tabn=%d and \
mesn=%d and godn=%d and datz <= '%d-%d-%d' and suma <> 0. \
order by datz,prn,knah asc",
 tabn,mn,gn,gk,mk,dk);
 
//fprintf(ff,"%s\n",strsql);
SQLCURSOR cur;
SQLCURSOR curr;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  fprintf(ff,"\n%s\n",gettext("Записей не найдено"));
  return;
 }

fprintf(ff,"\n%s:\n\
---------------------------------------------------------------------------\n",
gettext("Начисления"));

/*Распечатываем все начисления*/
sym=inah=iuder=iobud=itog=0;
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 1)
   continue;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  knah=atoi(row[3]);
  sym=atof(row[4]);

  memset(priz,'\0',sizeof(priz));  
  inah+=sym;
  strcpy(priz,"+");
  sprintf(strsql,"select naik from Nash where kod=%d",knah);
  memset(nai,'\0',sizeof(nai));
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);
/*
  printw("%2d %02d.%02d.%d %s %-30.30s %2s.%s %-4s %10s %s\n",
  knah,d,m,g,priz,nai,row[7],row[6],row[5],prnbr(sym),row[12]);
*/
  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai,
  row[7],
  row[6],
  iceb_u_kolbait(4,row[5]),row[5],
  iceb_u_prnbr(sym),row[12]);
  
 }
fprintf(ff,"%*s: %10s\n",iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(inah));

/*Распечатываем все обязятельные платежи*/

fprintf(ff,"\n%s:\n\
-------------------------------------------------------------------------\n",
gettext("Обязательные удержания"));
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 2)
   continue;
  knah=atoi(row[3]);
  if(obud != NULL)
   {
    for(i=1; i <= obud[0] ; i++)
     if(knah == obud[i])
      break;
    if(i > obud[0])
     { 
//      printw("Пропустили %d %d %d\n",i,obud[0],knah);
      continue;
     }
   }
  
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  sym=atof(row[4]);
  memset(priz,'\0',sizeof(priz));  

  iobud+=sym;
  strcpy(priz,"-");
  sprintf(strsql,"select naik from Uder where kod=%d",knah);

  memset(nai,'\0',sizeof(nai));
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);
/*
  printw("%2d %02d.%02d.%d %s %-30.30s %2s.%s %-4s %10s %s\n",
  knah,d,m,g,priz,nai,row[7],row[6],row[5],iceb_u_prnbr(sym),row[12]);
*/
  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai,
  row[7],
  row[6],
  iceb_u_kolbait(4,row[5]),row[5],
  iceb_u_prnbr(sym),row[12]);
  
 }
fprintf(ff,"%*s: %10s\n",iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(iobud));
fprintf(ff,"%*s: %10s\n",iceb_u_kolbait(58,gettext("К выдаче")),gettext("К выдаче"),iceb_u_prnbr(inah+iobud));

/*Распечатываем все удержания без обязательных платежей*/

fprintf(ff,"\n%s:\n\
------------------------------------------------------------------------\n",
gettext("Другие удержания"));
cur.poz_cursor(0);
while(cur.read_cursor(&row) != 0)
 {
  prn=atoi(row[2]);
  if(prn != 2)
   continue;
  knah=atoi(row[3]);
  if(obud != NULL)
   {
    for(i=1; i <= obud[0] ; i++)
     if(knah == obud[i])
      break;
    if(i <= obud[0])
     { 
//      printw("Пропустили %d %d %d\n",i,obud[0],knah);
      continue;
     }
   }

  iceb_u_rsdat(&d,&m,&g,row[0],2);
  sym=atof(row[4]);
  memset(priz,'\0',sizeof(priz));  

  iuder+=sym;
  strcpy(priz,"-");
  sprintf(strsql,"select naik from Uder where kod=%d",knah);

  memset(nai,'\0',sizeof(nai));
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    strncpy(nai,row1[0],sizeof(nai)-1);
/*
  printw("%2d %02d.%02d.%d %s %-30.30s %2s.%s %-4s %10s %s\n",
  knah,d,m,g,priz,nai,row[7],row[6],row[5],iceb_u_prnbr(sym),row[12]);
*/
  fprintf(ff,"%2d %02d.%02d.%d %s %-*.*s %2s.%s %-*s %10s %s\n",
  knah,d,m,g,priz,
  iceb_u_kolbait(30,nai),iceb_u_kolbait(30,nai),nai,
  row[7],
  row[6],
  iceb_u_kolbait(4,row[5]),row[5],
  iceb_u_prnbr(sym),row[12]);
  
 }
fprintf(ff,"%*s: %10s\n",iceb_u_kolbait(58,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(iuder));
fprintf(ff,"%*s: %10s\n",iceb_u_kolbait(58,gettext("Сальдо")),gettext("Сальдо"),iceb_u_prnbr(inah+iobud+iuder));

}
