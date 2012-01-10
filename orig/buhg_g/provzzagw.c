/* $Id: provzzagw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $ */
/*20.09.2006	26.05.2001	Белых А.И.	provzzagw.c
Проверка записан ли стандартный набор начислений/удержаний
и запись его если он не записан
*/
#include        "buhg_g.h"

void provzzagw(short mp,short gp,
int tabn,
int prn,  //1-начисление 2-удержание 3-и начисление и удержание
int podr,
GtkWidget *wpredok)
{
char		strsql[512];

if(prn == 1 || prn == 3)
 {
  sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%d-%d-1' and datz<='%d-%d-31' and tabn=%d and prn='1' limit 1",
  gp,mp,gp,mp,tabn);

  if(iceb_sql_readkey(strsql,wpredok) == 0)
   zapzagotw(mp,gp,tabn,1,podr,wpredok);
 }

if(prn == 2 || prn == 3)
 {
  sprintf(strsql,"select distinct tabn from Zarp where \
datz >= '%d-%d-1' and datz<='%d-%d-31' and tabn=%d and prn='2' limit 1",
  gp,mp,gp,mp,tabn);


  if(iceb_sql_readkey(strsql,wpredok) == 0)
    zapzagotw(mp,gp,tabn,2,podr,wpredok);
 }

}
