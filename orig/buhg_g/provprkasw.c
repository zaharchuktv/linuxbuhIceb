/* $Id: provprkasw.c,v 1.8 2011-02-21 07:35:56 sasa Exp $ */
/*09.03.2006	06.10.2000	Белых А.И.	provprkasw.c
Проверка выполнены ли проводки для кассового ордера
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"

extern SQL_baza bd;

void	provprkasw(const char kassa[],short tipz,const char nomd[],short dd,
short md,short gd,const char kodop[],
GtkWidget *wpredok)
{
char		strsql[512];
int		kolstr;
SQL_str		row;
SQLCURSOR curr;
double		sumd,sumpod;
double		sum;
iceb_u_str repl;

SQLCURSOR cur;

/*Проверяем код операции*/

if(tipz == 1)
 sprintf(strsql,"select metkapr from Kasop1 where kod='%s'",kodop);
if(tipz == 2)
 sprintf(strsql,"select metkapr from Kasop2 where kod='%s'",kodop);

if(sql_readkey(&bd,strsql,&row,&curr) == 1)
  if(row[0][0] == '0')
    goto vper;
    
if(tipz == 1)
 sprintf(strsql,"select deb \
from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%d-%02d-%02d' and deb != 0. and tz=%d",
 gettext("КО"),kassa,nomd,kodop,gd,md,dd,tipz);

if(tipz == 2)
 sprintf(strsql,"select kre \
from Prov where kto='%s' and pod=%s and nomd='%s' and oper='%s' and \
datd='%d-%02d-%02d' and kre != 0. and tz=%d",
 gettext("КО"),kassa,nomd,kodop,gd,md,dd,tipz);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
  goto vper1;  

sumkasorw(kassa,tipz,nomd,gd,&sumd,&sumpod,wpredok);
sum=0.;
while(cur.read_cursor(&row) != 0)
  sum+=atof(row[0]);


if(fabs(sum-sumpod) > 0.009)
 {
  repl.new_plus(gettext("Не сделаны все проводки !"));  
  
  sprintf(strsql,"%-30s: %8.2f",
  gettext("Подтвержденная сумма"),sumpod);  
  repl.ps_plus(strsql);

  sprintf(strsql,"%-30s: %8.2f",
  gettext("Сумма выполненых проводок"),sum);  
  repl.ps_plus(strsql);

  iceb_menu_soob(&repl,wpredok);

vper1:;

  sprintf(strsql,"update Kasord set \
prov=0 where kassa=%s and tp=%d and nomd='%s' and god=%d",
  kassa,tipz,nomd,gd);
 }
else
 {

vper:;

  sprintf(strsql,"update Kasord set \
prov=1 where kassa=%s and tp=%d and nomd='%s' and god=%d",
  kassa,tipz,nomd,gd);
 }
if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   char bros[512];
   sprintf(bros,"provprkas-%s",gettext("Ошибка корректировки записи !"));  
   iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
  }
}

