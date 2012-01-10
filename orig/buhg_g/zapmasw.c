/* $Id: zapmasw.c,v 1.4 2011-02-21 07:35:59 sasa Exp $ */
/*04.09.2006      12.04.1994    Белых А.И.      zapmasw.c
Заполнение массива начислений и удержаний на одного человека
Если вернули 0 зарплата начислена
	     1 нет
*/
#include <stdlib.h>
#include        "buhg_g.h"
#include        "zarp1.h"

struct ZAR	zar;
extern SQL_baza bd;

int zapmasw(int tnn,short m,short g,GtkWidget *wpredok)
{
short           i;
long		kolstr;
SQL_str         row;
char		strsql[400];
short		d1,m1,g1;

memset(&zar,'\0',sizeof(zar));

sprintf(strsql,"select prn,knah,suma,mesn,godn,kdn,shet,datz from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma <> 0. order by prn,knah asc",g,m,g,m,tnn);
/*
printw("%s\n",strsql);
refresh();
*/
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(1);

i=0;
while(cur.read_cursor(&row) != 0)
 {
//  printw("%s %s %s %s\n",row[0],row[1],row[2],row[3]);
//  refresh();
  if(i == razm)
   {
    iceb_menu_soob("zapmasw-Массив меньше количества начислений и удержаний !",wpredok);
    return(0);
   }

  zar.prnu[i]=atoi(row[0]);
  zar.knu[i]=atoi(row[1]);
  zar.sm[i]=atof(row[2]);
  zar.mes[i]=atoi(row[3]);
  zar.god[i]=atoi(row[4]);
  zar.kolbd[i]=atoi(row[5]);
  strcpy(zar.sheta[i],row[6]);
  iceb_u_rsdat(&d1,&m1,&g1,row[7],2);
  zar.denz[i]=d1;
  i++;
 }
/****************
printw("\n");
for(i=0;i<razm;i++)
 {
  printw("%d %d.%d %d %d %f\n",i,zar.mes[i],zar.god[i],zar.prnu[i],zar.knu[i],zar.sm[i]);
  if(zar.mes[i] == 0)
   break;
 }
OSTANOV();
*************/
return(0);
}
