/* $Id: dopdominw.c,v 1.7 2011-04-14 16:09:35 sasa Exp $ */
/*22.12.2010	22.08.2000	Белых А.И.	dopdominw.c
Расчет доплаты до минимальной зарплаты
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"

extern double   okrg;
extern short	koddopmin; /*Код доплаты до минимальной зарплаты*/
extern short    *kodnvmin; /*Коды не входящие в расчет доплаты до минимальной зарплаты*/
extern short    *kodotrs;  /*Коды отдельного рачета доплаты до минимальной зарплаты*/
extern float    dnei; /*Количество рабочих дней в текущем месяце*/
extern SQL_baza bd;

void dopdominw(int tabn,short mp,short gp,int podr,
double koefpv,const char *nah_only,FILE *ff_prot,GtkWidget *wpredok)
{
double		sum,suma;
double		doplata;
int		i1;
char		shet[32];
short		d;
char		strsql[512];
long		kolstr;
SQL_str         row;
int		knah;
short           nomz;
struct ZARP     zp;
class SQLCURSOR cur;

/*
printw("\ndopdomin-%d %d %d %d %f koddopmin=%d\n",
tabn,mp,gp,podr,koefpv,koddopmin);
OSTANOV();
*/
class zar_read_tn1h nastr;
zar_read_tn1w(1,mp,gp,&nastr,ff_prot,wpredok);

if(ff_prot != NULL)
 fprintf(ff_prot,"\nРасчет начисления до минимальной зарплаты:%.2f\n\
--------------------------------------------------------\n",nastr.minzar); 

if(koddopmin == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код начисления до минимальной зарплаты равен нолю!\n");  
  return;
 }

if(iceb_u_proverka(nah_only,koddopmin,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",koddopmin);
  return;
 }

/*Проверяем есть ли код доплаты в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tabn,koddopmin); 
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
  return;
strncpy(shet,row[0],sizeof(shet)-1);

sprintf(strsql,"select knah,suma from Zarp where datz>='%d-%d-01' and \
datz <= '%d-%d-31' and tabn=%d and prn='1' and knah != %d and \
godn=%d and mesn=%d and suma <> 0.",
gp,mp,gp,mp,tabn,koddopmin,gp,mp);

//printw("%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

//printw("kolstr=%d\n",kolstr);
//OSTANOV();
if(kolstr == 0)
  return;


/*Вычисляем начисленную сумму*/
iceb_u_dpm(&d,&mp,&gp,5);
nomz=1;
suma=0.;
while(cur.read_cursor(&row) != 0)
 {
  knah=atoi(row[0]);
  sum=atof(row[1]);
  /*Проверяем на коды не входящие в расчет*/
  if(kodnvmin != NULL)
   {
    for(i1=1; i1 <= kodnvmin[0]; i1++)
     if(knah == kodnvmin[i1])
      break;
    if(i1 <= kodnvmin[0])
     continue;
   }  

  /*Проверяем на коды отдельного расчета доплаты до минимальной зарплаты*/
  if(kodotrs != NULL)
   {
    for(i1=1; i1 <= kodotrs[0]; i1++)
     if(knah == kodotrs[i1])
      break;
    if(i1 <= kodotrs[0])
      continue;
   }  
  suma+=sum;
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"Сумма которая берется для расчета:%.2f\n",suma);

if(koefpv > 1.)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"Устанавливаем koefvp=1.\n");
  koefpv=1.;
 } 

//Узнаем количество отработанных дней
float	ddd=0.;
sprintf(strsql,"select dnei from Ztab where god=%d and mes=%d and \
tabn=%d and kodt=1",gp,mp,tabn);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
  ddd=atof(row[0]);     

//doplata=nastr.minzar*koefpv-suma;
doplata=nastr.minzar/dnei*ddd-suma;
if(ff_prot != NULL)
 fprintf(ff_prot,"%.2f/%.2f*%.2f-%.2f=%.2f\n",nastr.minzar,dnei,ddd,suma,doplata);

if(doplata < 0.01)
  doplata=0.;

doplata=iceb_u_okrug(doplata,okrg);


zp.tabnom=tabn;
zp.prn=1;
zp.knu=koddopmin;
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
strcpy(zp.shet,shet);

zapzarpw(&zp,doplata,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//zapzarp(d,mp,gp,tabn,1,koddopmin,doplata,shet,mp,gp,0,0,kom,podr,"",zp); 

}
