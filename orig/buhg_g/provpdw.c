/* $Id: provpdw.c,v 1.8 2011-02-21 07:35:56 sasa Exp $ */
/*29.09.2006	20.05.1998	Белых А.И.	provpdw.c
Проверка выполнены ли все проводки по платежному поручению
Смотрим на какую сумму подтверждена платежка 
если меньше чем в документе то документ не подтвержден
если сумма проводок равна подтвержденной сумме то проводки
выполнены
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "dok4w.h"

extern struct REC rec;
extern SQL_baza bd;

void provpdw(char tabl[],GtkWidget *wpredok)
{
double		sum,sump,sumpp;
char		shet[32];
short		pr,podt;
short		dp,mp,gp;
long		kolstr;
SQL_str         row;
char		strsql[512];
char		bros[512];
SQLCURSOR cur;  

pr=1;
sump=sumpzpdw(tabl,&dp,&mp,&gp,&sumpp,wpredok);

if(fabs(sump -(rec.sumd+rec.uslb)) > 0.0099)
 {
  podt=0;
 }
else
 podt=1;

if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"%s","avtprodok.alx");

if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"%s","avtprodokt.alx");

memset(shet,'\0',sizeof(shet));
if(iceb_poldan(rec.nsh.ravno(),shet,strsql,wpredok) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден бухгалтерский счёт для счёта"));
  repl.plus(" ");
  repl.plus(rec.nsh.ravno());
  repl.ps_plus(gettext("Файл"));
  repl.plus(" ");
  repl.plus(strsql);
  repl.plus(" ");
  repl.plus(gettext("Проводки не проверялись !"));
  iceb_menu_soob(&repl,wpredok);
  goto vp;
 } 

/*Суммиреум выполненные проводки*/
if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
 sprintf(strsql,"select sh,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
 gettext("ПЛТ"),0,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd);

if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
 sprintf(strsql,"select sh,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%02d-%02d'",
 gettext("ТРЕ"),0,rec.nomdk.ravno(),rec.gd,rec.md,rec.dd);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  pr=1;
  goto vp;
 }

sum=0.;
pr=0;
while(cur.read_cursor(&row) != 0)
 {

  pr++;
  
  if(iceb_u_SRAV(shet,row[0],0) == 0 )
    sum+=atof(row[1]);

 }


if(pr== 0)
 {
  pr=1;
  goto vp;
 }

pr=0;
rec.prov=0;
if(fabs(sum-sump) > 0.00999)
 {
  
  sprintf(strsql,"%s %.2f != %.2f",gettext("Не выполнены все проводки !"),sum,sump);
  iceb_menu_soob(strsql,wpredok);
  rec.prov=1;
  pr=1;
 }

vp:;

sprintf(strsql,"update %s \
set \
prov=%d,\
podt=%d \
where datd='%d-%02d-%02d' and nomd='%s'",
tabl,pr,podt,rec.gd,rec.md,rec.dd,rec.nomdk.ravno());

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  { 
   sprintf(bros,"provpdw-%s",gettext("Ошибка корректировки записи !"));
   iceb_msql_error(&bd,bros,strsql,wpredok);
  }
}
