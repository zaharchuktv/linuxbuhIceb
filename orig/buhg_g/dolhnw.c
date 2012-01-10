/* $Id: dolhnw.c,v 1.10 2011-04-14 16:09:35 sasa Exp $ */
/*28.11.2008    06.01.1993      Белых А.И.      dolhnw.c
Автоматизированное начисление за должность с учетом
отработанного времени
Если в конце стоит буква Ч значит это не оклад а
почасовая ставка
Возвращяем коэффициент полноты выплаты
*/
#include <stdlib.h>
#include        <ctype.h>
#include        <errno.h>
#include        "buhg_g.h"

extern double   okrg;
extern float    hasov; /*Количество рабочих часов в текущем месяце*/
extern float    dnei;
extern SQL_baza bd;
extern double minzar;

double dolhnw(int tn, //Табельный номер
short mp,short gp, //Дата
int kp,  //Код подразделения
const char *kom, //Коментарий
const char *nah_only,
FILE *ff_prot, //Файл протокола
GtkWidget *wpredok)
{
double		koefpv;
short           d;
int             kodokl; /*Код должносного оклада*/
double          oklad;  /*Оклад*/
double          nah;
int             met; /*0-оклад 1-часовая ставка 2-не пересчитываемый оклад*/
SQL_str         row;
SQLCURSOR curr;
char		strsql[512];
float		dneii,has;
float		kolrd; /*Количество рабочих дней в месяце*/
char		shet[56];
float		has1;   /*Количество часов в одном рабочем дне*/
float		kof;   /*Коэффициент оплаты ставки*/
float		mesnoh;  /*Месячная норма отработанных часов*/
double		procent=0.,procb=0.;
int		metka; /*0-нет метки 1-есть Метка пересчета на процент выполнения плана подразделением*/
int		metka1; /*0-расчет выполнять по отработанным часам 1-дням*/
int		nomz=0;
class ZARP     zp;
int		kodtab=0;
int metka_no=0;
koefpv=oklad=nah=0.;

/*
printw("dolhn-%d %d.%d %d\n",tn,mp,gp,kp);
refresh();
*/
memset(shet,'\0',sizeof(shet));
kolrd=dnei;
met=0;

if(dolhn1w(tn,&oklad,&has1,&kof,&met,&kodokl,&metka,&metka1,shet,&metka_no,wpredok) != 0)
  return(koefpv);

if(shet[0] == '*')
 {
  //читаем счёт в карточке
  sprintf(strsql,"select shet from Kartb where tabn=%d",tn);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
   strncpy(shet,row[0],sizeof(shet)-1);
  else
  memset(shet,'\0',sizeof(shet));
 }
if(shet[0] != '\0')
 {
  OPSHET opshet;
  if(iceb_prsh1(shet,&opshet,wpredok) != 0)
    memset(shet,'\0',sizeof(shet));
   
  
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"\nРасчёт от должностного оклада.\n-------------------------------\n\
Табельный номер:%d dolhn-dnei=%.2f hasov=%.2f oklad=%.2f has1=%.2f kof=%.2f met=%d kodok=%d metka_no=%d\n",
  tn,dnei,hasov,oklad,has1,kof,met,kodokl,metka_no);

if(iceb_u_proverka(nah_only,kodokl,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodokl);
  return(koefpv);
   
 }

if(met == 2)
 oklad=okladtrw(mp,gp,oklad,ff_prot,wpredok);

if(kodokl == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s\n",gettext("dolhn-Не введён код оклада !!!"));
  return(koefpv);
 }



/*Проверяем есть ли код начисления в списке*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,kodokl); 
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не нашли код %d в списке начислений для %d табельного номера !\n",
   kodokl,tn);
  return(koefpv);
 }
if(shet[0] == '\0')
  strncpy(shet,row[0],sizeof(shet)-1);

/*Читаем код табеля*/
sprintf(strsql,"select kodt from Nash where kod=%d",
kodokl);
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  if(ff_prot != NULL)
    fprintf(ff_prot,"Нет кода начисления %d в списке начислений !\n",kodokl);
  return(koefpv);
 }
kodtab=atoi(row[0]);
if(ff_prot != NULL)
  fprintf(ff_prot,"Код табеля для автоматизированного расчета %d\n",
  kodtab);

/*Читаем табель*/
if(kom[0] == '\0')
 sprintf(strsql,"select dnei,has,kolrd,kolrh,nomz from Ztab \
where god=%d and mes=%d and tabn=%d and kodt=%d order by nomz asc",
 gp,mp,tn,kodtab);
else
 sprintf(strsql,"select dnei,has,kolrd,kolrh,nomz from Ztab \
where god=%d and mes=%d and tabn=%d and kodt=%d and kom='%s' \
order by nomz asc",gp,mp,tn,kodtab,kom);

if(sql_readkey(&bd,strsql,&row,&curr) == 0)
 {
  dneii=has=0.;
  kolrd=0.;
  nomz=0;
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найден код табеля %d указанного в начислении.\n",kodtab);
 }
else
 {
  nomz=atoi(row[4]);
  dneii=atof(row[0]);
  has=atof(row[1]);
  if(atof(row[2]) != 0.)
   {
    kolrd=atof(row[2]);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Количество рабочих дней=%.2f\n",kolrd);
   }
  if(atof(row[3]) != 0.)
   {
    has1=atof(row[3]);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Количество часов в смене=%.2f\n",has1);
   }
 }

mesnoh=hasov;
if(has1 != 0.)
 {
  mesnoh=kolrd*has1;
  if(ff_prot != NULL)
   fprintf(ff_prot,"mesnoh=%.2f*%.2f=%.2f\n",kolrd,has1,mesnoh);
 }  
else
  if(ff_prot != NULL)
   fprintf(ff_prot,"mesnoh=%.2f\n",mesnoh);
/*
printw("\nmet=%d metka1=%d kolrd=%d\n",met,metka1,kolrd);
OSTANOV();
*/
nah=0.;
if(metka_no == 1)
 {
  nah=oklad;
  koefpv=1.;
 }
else
 {
  if(met == 0 || met == 2)
   {
    if(mesnoh > 0.00001 && metka1 == 0)
     {
      nah=oklad/mesnoh*has;
      koefpv=has/mesnoh;
      if(ff_prot != NULL)
         fprintf(ff_prot,"Начислено(по часам)=%.2f/%.2f*%.2f=%.2f koefpv=%.2f/%.2f=%f\n",
         oklad,mesnoh,has,nah,has,mesnoh,koefpv);
     }
    if(metka1 == 1 && kolrd > 0)
     {
      nah=oklad/kolrd*dneii;
      koefpv=dneii/kolrd;
      if(ff_prot != NULL)
         fprintf(ff_prot,"Начислено(по дням)=%.2f/%.2f*%.2f=%.2f koefpv=%f\n",
         oklad,kolrd,dneii,nah,koefpv);
     }   
   }
  if(met == 1)
   {
    nah=oklad*has;
    koefpv=1.;
     if(ff_prot != NULL)
       fprintf(ff_prot,"Начислено=%.2f*%.2f=%.2f koefpv=%f\n",
       oklad,has,nah,koefpv);
   }
 }

if(metka == 1)
 {
  
  procent=zarpodprw(kp,&procb,wpredok);
  if(ff_prot != NULL)
   fprintf(ff_prot,"Учет процента выполнения плана подразделением %d, процент: %.2f %%\n",
   kp,procent);   
  if(ff_prot != NULL)
     fprintf(ff_prot,"%.2f*%.2f/100=",
     nah,procent);
  koefpv*=procent/100.;
  nah=nah*procent/100.;
  if(ff_prot != NULL)
     fprintf(ff_prot,"%.2f koefpv=%f\n",nah,koefpv);

 } 

if(nah > 0.)
  nah=iceb_u_okrug(nah,okrg);
/*
if(ff_prot != NULL)
  fprintf(ff_prot,"%s: %.2f (%s:%.2f/%.2f %s:%.2f %s:%.2f)\n\n",
gettext("Начислено"),
nah,
gettext("Часов"),
mesnoh,has,
gettext("Дней"),dneii,
gettext("Оклад"),oklad);
*/
iceb_u_dpm(&d,&mp,&gp,5);


zp.tabnom=tn;
zp.prn=1;
zp.knu=kodokl;

zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=kp;
strcpy(zp.shet,shet);

zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,kp,"",wpredok);
//zapzarp(d,mp,gp,tn,1,kodokl,nah,shet,mp,gp,0,nomz,kom,kp,"",zp); 

return(koefpv);
}
