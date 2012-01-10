/* $Id: ostdok1w.c,v 1.5 2011-01-13 13:49:52 sasa Exp $ */
/*07.04.2003	22.02.1999	Белых А.И.	ostdok1w.c
Получение остатка материалла исходя только из записей в документах
в том числе не привязанных к карточкам и из таблицы розничной тоговли
Возвращяет значение остатка материалла
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <math.h>
#include	"buhg_g.h"

extern short	startgod; /*Стартовый год просмотров*/
extern SQL_baza	bd;

double 		ostdok1w(short dk,short mk,short gk,int skl,int kodm)
{
SQL_str         row;
char		strsql[512];
int		kolstr;
double		kolih=0.;
double		suma=0.;
short		tp;
short		godn;
short		d,m,g;

godn=startgod;
if(startgod == 0 || gk < startgod )
 godn=gk;

/* 
printw("ostdok1 начал работу godn=%d startgod=%d\n\
%d.%d.%d skl=%d kodm=%d",godn,startgod,dk,mk,gk,skl,kodm);
OSTANOV();
*/

if(skl != 0)
 sprintf(strsql,"select datd,nomd,kolih,tipz from Dokummat1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and kodm=%d and \
sklad=%d order by datd asc",
 godn,1,1,gk,mk,dk,kodm,skl);
else
 sprintf(strsql,"select datd,nomd,kolih,tipz from Dokummat1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and kodm=%d \
order by datd asc",
 godn,1,1,gk,mk,dk,kodm);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 iceb_msql_error(&bd,"ostdok1w",strsql,NULL);

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  /*Пpопускаем стаpтовые остатки если это остатки не года начала поиска*/
  if(iceb_u_SRAV(row[1],"000",0) == 0 && g != godn)
   continue;

  tp=atoi(row[3]);
  suma=atof(row[2]);
  if(tp == 1)
    kolih+=suma;
  if(tp == 2)
    kolih-=suma;

 }

/*
printw("Просмотр таблицы реализации.\n");
refresh();
*/

if(skl != 0)
 sprintf(strsql,"select kolih from Roznica where \
datrp >= '%04d-%02d-%02d' and datrp <= '%04d-%02d-%02d' and metka=1 \
and kodm=%d and sklad=%d",
 gk,1,1,gk,mk,dk,kodm,skl);
else
 sprintf(strsql,"select kolih from Roznica where \
datrp >= '%04d-%02d-%02d' and datrp <= '%04d-%02d-%02d' and metka=1 and \
kodm=%d",
 gk,1,1,gk,mk,dk,kodm);
//printw("%s\n",strsql);
//OSTANOV();
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 iceb_msql_error(&bd,"ostdok1w",strsql,NULL);

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
  kolih-=atof(row[0]);

/*
printw("Закончил %d %f\n",kodm,kolih);
OSTANOV();
*/

if(fabs(kolih) < 0.000000001)
  kolih=0.;
return(kolih);
 
}
