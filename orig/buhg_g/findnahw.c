/* $Id: findnahw.c,v 1.5 2011-02-21 07:35:51 sasa Exp $ */
/*04.09.2006    10.12.1993      Белых А.И.      findnahw.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/

#include <stdlib.h>
#include        "buhg_g.h"

extern short    *knvr;/*Коды начислений не входящие в расчет налогов*/
extern  short   *kodbl; /*Код больничного*/
extern SQL_baza bd;

double findnahw(int tnn, //Табельный номер
short m,short g, //Дата
GtkWidget *wpredok)
{
double          nah;
long		kolstr;
SQL_str         row;
char		strsql[512];
short		knah;

//printw("findhah-%d %d.%d\n",tnn,m,g);

sprintf(strsql,"select knah,suma,mesn from Zarp where \
datz >= '%d-%02d-01' and datz <= '%d-%02d-31' and tabn=%d and prn='1'",
g,m,g,m,tnn);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr <= 0)
  return(0.);

nah=0.;

while(cur.read_cursor(&row) != 0)
 {

//  printw("%s %s %s\n",row[0],row[1],row[2]);
  
  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчета пенсионого и профсоюзного взносов включается*/
  knah=atoi(row[0]);
  if(provkodw(kodbl,knah) >= 0 && atoi(row[2]) != m)
     continue;

  nah+=atof(row[1]);
 }
return(nah);
}
