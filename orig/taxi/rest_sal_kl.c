/*$Id: rest_sal_kl.c,v 1.6 2011-02-21 07:36:21 sasa Exp $*/
/*13.08.2005	13.08.2005	Белых А.И.	rest_sal_kl.c
Получение конечного сальдо по клиенту
*/
#include <stdlib.h>
#include "i_rest.h"

extern short start_god_rest;
extern SQL_baza bd;

double rest_sal_kl(const char *kodkl,GtkWidget *wpredok)
{
char strsql[500];


SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datz,nomd,suma from Restkas where datz >= '%04d-01-01' \
and kodkl='%s'",start_god_rest,kodkl);

int kolstr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  exit(1);
 }

short d,m,g;
double saldo=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  if(start_god_rest < g && iceb_u_SRAV("000",row[1],0) == 0)
   continue;
  saldo+=atof(row[2]);
 }


sprintf(strsql,"select nomd,datd,podr from Restdok where kodkl='%s' and mo=0",
kodkl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(saldo);
 }

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[1],2);

  saldo-=sumapsh(g,row[0],NULL,wpredok);

 }



return(saldo);
}
