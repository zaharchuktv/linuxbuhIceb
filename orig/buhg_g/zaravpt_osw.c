/*$Id: zaravpt_osw.c,v 1.1 2011-03-15 09:28:17 sasa Exp $*/
/*12.03.2011	12.03.2011	Белых А.И.	zaravpt_osw.c
Программа формирования масивов объединенных одинаковых строк
Если строк несколько то при выполнении проводок по каждой строке
возникает погрешность при округлении до копеек . Если одинаковые строки объединить
 то погрешность будет меньше.
***/
#include <stdlib.h>
#include "buhg_g.h"
extern SQL_baza bd;

int  zaravpt_osw(int tbn,short mp,short gp,class iceb_u_spisok *data,class iceb_u_double *suma,GtkWidget *wpredok)
{
SQL_str row1;
int kolstr2;
char strsql[512];

SQLCURSOR cur1;
/*********
sprintf(strsql,"select datz,suma,shet,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma != 0. and prn='1' order by knah asc",
gp,mp,gp,mp,tbn);
***********/
sprintf(strsql,"select datz,suma,shet,knah,kom from Zarp where \
datz >='%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and \
suma != 0. and prn='1' order by suma asc",
gp,mp,gp,mp,tbn);

if((kolstr2=cur1.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr2 <= 0)
  return(1);
int nomer=0;
while(cur1.read_cursor(&row1) != 0)
 {
  sprintf(strsql,"%s|%s|%s|%s|",row1[0],row1[2],row1[3],row1[4]);
  if((nomer=data->find(strsql)) < 0)
     data->plus(strsql);
  
  suma->plus(atof(row1[1]),nomer);
 }
 
return(0);
}
