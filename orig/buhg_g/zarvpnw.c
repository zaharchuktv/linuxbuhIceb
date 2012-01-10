/*$Id: zarvpnw.c,v 1.5 2011-02-21 07:36:00 sasa Exp $*/
/*22.12.2010	18.07.2008	Белых А.И.	zarvpnw.c
Подпрограмма возврата подоходного налога
*/
#include <stdlib.h>
#include "buhg_g.h"

extern short kodpn;   /*Код подоходного налога*/
extern short kodvpn; /*Код возврата подоходного налога*/
extern SQL_baza bd;

void zarvpnw(int tabn,
short mes,short god,
int podr,
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
if(kodvpn == 0)
 return;
 
char strsql[512];
short		d;

iceb_u_dpm(&d,&mes,&god,5);


if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nВозврат подоходного налога\n\
---------------------------------------------------\n");
 }

if(iceb_u_proverka(uder_only,kodvpn,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodvpn);
  return;
 }
 
SQL_str row;
class SQLCURSOR cur;

/*Проверяем есть ли код возврата подоходного налога*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tabn,kodvpn);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) <= 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введено начисление возврата подоходного налога %d !\n",kodvpn);
  return;
 }
char shet[32];
strncpy(shet,row[0],sizeof(shet)-1);

int kolstr=0;
/*Читаем сумму подоходного налога*/
sprintf(strsql,"select suma from Zarp where tabn=%d and \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and \
prn='2' and knah=%d and suma <> 0.",
tabn,god,mes,god,mes,kodpn);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }
if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено записей с подоходным налогом!\n");
  return;
 }

double suma_podoh=0.;
while(cur.read_cursor(&row) != 0)
 suma_podoh+=atof(row[0]);
 
if(ff_prot != NULL)
 fprintf(ff_prot,"Сумма подоходного налога=%.2f\n",suma_podoh);

struct ZARP     zp;

zp.tabnom=tabn;
zp.prn=1;
zp.knu=kodvpn;
zp.dz=d;
zp.mz=mes;
zp.gz=god;
zp.mesn=mes; zp.godn=god;
zp.nomz=0;
zp.podr=podr;
strncpy(zp.shet,shet,sizeof(zp.shet)-1);

//zapzarpw(d,mes,god,tabn,1,kodvpn,suma_podoh*-1,shet,mes,god,0,0,"",podr,"",zp,wpredok);
zapzarpw(&zp,suma_podoh*-1,d,mes,god,0,shet,"",0,podr,"",wpredok);
 

}




