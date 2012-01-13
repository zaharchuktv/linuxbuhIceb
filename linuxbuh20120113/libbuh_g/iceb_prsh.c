/* $Id: iceb_prsh.c,v 1.10 2011-02-21 07:36:07 sasa Exp $ */
/*20.04.2009    16.11.1993      Белых А.И.      iceb_prsh.c
Подпрограмма проверки наличия счета
Если вернули 0 - счета нет
	     1 - счет есть и не имеет субсчетов
	     2 - счет есть и имеет субсчета
	     3 - счет является субсчетом
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_prsh(const char *ssh, //Номер счета
OPSHET *shetv, //Возвращаем тип сальдо
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str         row;
int		kolstr;
char		shet[40];
short	vplsh=0; /*0-двух уровневый план счетов 1-многоуровневый*/

if(iceb_poldan("Многопорядковый план счетов",strsql,"nastrb.alx",wpredok) == 0)
 if(iceb_u_SRAV("Вкл",strsql,1) == 0)
  vplsh=1;

strncpy(shet,ssh,sizeof(shet)-1);
sqlfiltr(shet,sizeof(shet));

SQLCURSOR cur;
sprintf(strsql,"select * from Plansh where ns like '%s%%' \
order by ns asc",shet);
/*
printw("strsql=%s\n",strsql);
refresh();
*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  char bros[512];
  sprintf(bros,"prsh-%s",gettext("Ошибка создания курсора !"));
  iceb_msql_error(&bd,bros,strsql,NULL);
 }


if(kolstr == 0)
  return(0);

cur.read_cursor(&row);

shetv->naim.new_plus(row[1]);
shetv->tips=atoi(row[2]);
shetv->vids=atoi(row[3]);
shetv->saldo=atoi(row[6]);
strncpy(shet,row[0],sizeof(shet));
shetv->stat=atoi(row[7]);
shetv->val=atoi(row[8]);

if(kolstr == 1 && iceb_u_SRAV(ssh,shet,0) != 0)
  return(0);


if(kolstr == 1 && iceb_u_SRAV(ssh,shet,0) == 0 && shetv->vids == 0)
   return(1);

/*Для двух уровневой системы счетов*/
if(vplsh == 0)
 if(kolstr >= 1 && iceb_u_SRAV(ssh,shet,0) == 0 && shetv->vids == 1)
    return(3);

/*Многоуровневый план счетов*/
if(vplsh == 1)
 {
  if(kolstr == 1 && iceb_u_SRAV(ssh,shet,0) == 0 && shetv->vids == 1)
      return(3);
  if(kolstr > 1 && iceb_u_SRAV(ssh,shet,0) == 0 && shetv->vids == 1)
      return(2);
 }
 
if(kolstr > 1 && iceb_u_SRAV(ssh,shet,0) == 0)
 return(2);


return(0);
}
