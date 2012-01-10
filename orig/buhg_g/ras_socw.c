/*$Id: ras_socw.c,v 1.8 2011-04-14 16:09:36 sasa Exp $*/
/*22.12.2010	17.02.2005	Белых А.И.	ras_socw.c
Расчёт соц-стаха для работника
*/
#include        "buhg_g.h"

extern short	metkarnb; //0-Расчет налогов для бюджета пропорциональный 1-последовательный
extern short	*kodsocstrnv; //Коды не входящие в расчет соцстраха
extern short    kodsocstr;
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetsoc; //Счета фонда соц-страха
extern double   okrg; /*Округление*/


void ras_socw(int tn, //Табельный номер*/
short mp,short gp,
int podr,
double *sumaos, //Сумма отчисления на соц-страх только с зарплаты
double *sumaosb, //Сумма отчисления на соц-страх бюджет только с зарплаты
int metka_zap, //0-делать запись 1-нет
int metka_ras, //0-смотреть начисления в массиве 1-в базе
double suma_boln, 
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR curr;
char shet[32];
double suma_boln_rm=0.;
class ZARP     zp;
short		d;

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=kodsocstr;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
*sumaos=*sumaosb=0.;

/*Проверяем есть ли код удержания отчисления в соцстах*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,kodsocstr);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) <= 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"\nНе введён код отчисления в соц-страх %d !\n",kodsocstr);
  return;
 }

double sumanb=0.;
double sumabu=0.;
double suma_ndfnz=0.;
double suma_ndfnz_b=0.;
double suma_zarp_o=0.;
double suma_zarp_b=0.;

if(ff_prot != NULL)
 fprintf(ff_prot,"\n%s-Расчет отчисления на соц-страх.\n\
---------------------------------------------------\n",__FUNCTION__);
strncpy(shet,row[0],sizeof(shet)-1);

if(iceb_u_proverka(uder_only,kodsocstr,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodsocstr);
  return;
 }

double soc_suma=0.;
if(metka_ras == 0)
  soc_suma=findnahmw(mp,5,kodsocstrnv,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm);
else
 {
  soc_suma=findnahmw(mp,gp,tn,5,kodsocstrnv,ff_prot,&sumanb,&sumabu,&suma_ndfnz,&suma_ndfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm,wpredok);
  //если есть сyмма больничного значит делаем перерасчёт подоходного для больничных на которые 
  //распространяется социальная льгота 
  if(suma_boln != 0.)
   if(ff_prot != NULL)
    fprintf(ff_prot,"Увеличиваем на сумму больничного\n");
  soc_suma+=suma_boln;
  sumanb+=suma_boln;
  suma_zarp_o+=suma_boln;
 }
 
if(ff_prot != NULL)
 fprintf(ff_prot,"findnahm - nah=%f Хозрасчет=:%.2f Бюджет=%.2f\n",
  soc_suma,sumanb,sumabu);

//rudnfvw(&soc_suma,suma_ndfnz,ff_prot);

double soc_suma_b=sumabu;

if(ff_prot != NULL)
 fprintf(ff_prot,"С общей суммы.\n");

class zar_read_tn1h nastr;
zar_read_tn1w(1,mp,gp,&nastr,ff_prot,wpredok);
 
double bb1=zar_rsocw(soc_suma,nastr.prog_min_tek,ff_prot);
if(ff_prot != NULL)
 fprintf(ff_prot,"Только с зарплаты.\n");
*sumaos=zar_rsocw(suma_zarp_o,nastr.prog_min_tek,ff_prot);

if(ff_prot != NULL)
  fprintf(ff_prot,"Налог=%.2f\n",bb1);

//  printw("bb1=%f\n",bb1);
char bros[512];
memset(bros,'\0',sizeof(bros));
if(shetb == NULL || shetsoc == NULL)
 {
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"bb1=%f kodsocsrt=%d\n",bb1,kodsocstr);
  
  strcpy(zp.shet,shet);
  if(metka_zap == 0)
   zapzarpw(&zp,bb1,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//    zapzarp(d,mp,gp,tn,2,kodsocstr,bb1,shet,mp,gp,0,0,bros,podr,"",zp);
 }
else
 {
  double bb2=0.;
  if(metkarnb == 1) //Последовательный метод расчет налогов
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Расчёт для бюджетных начислений.\n");
//    rudnfvw(&soc_suma_b,suma_ndfnz_b,ff_prot);
    if(ff_prot != NULL)
      fprintf(ff_prot,"С общей суммы.\n");

    bb2=zar_rsocw(soc_suma_b,nastr.prog_min_tek,ff_prot);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Только с зарплаты.\n");
    *sumaosb=zar_rsocw(suma_zarp_b,nastr.prog_min_tek,ff_prot);
   } 

  if(metkarnb == 0) //Пропорциональный метод расчет налогов
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"Расчёт для бюджетных начислений.\n");
      fprintf(ff_prot,"Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f \
Налог:%.2f Счета:%s\n",
      soc_suma,sumanb,sumabu,bb1,shetsoc);
     }
    if(sumabu != 0.)
     {
      bb2=0.;
      //Если хозрасчет не равен нолю то долю определять надо
      if(sumanb != 0.)
       {
        if(soc_suma != 0.)
         bb2=bb1*sumabu/soc_suma;
       }
      else
       bb2=bb1;
     }
    bb2=iceb_u_okrug(bb2,okrg);

    if(suma_zarp_b != 0.)
       *sumaosb=*sumaos*suma_zarp_b/suma_zarp_o;
    else 
       *sumaosb=0.;  
    *sumaosb=iceb_u_okrug(*sumaosb,okrg);
   }
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"Налог бюджет=%.2f\n",bb2);
    
  iceb_u_polen(shetsoc,shet,sizeof(shet),2,',');
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Бюджет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
     zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodsocstr,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }

  iceb_u_polen(shetsoc,shet,sizeof(shet),1,',');
  bb2=bb1-bb2;
  bb2=iceb_u_okrug(bb2,okrg);
  if(ff_prot != NULL)
    fprintf(ff_prot,"Налог хозрасчет=%.2f\n",bb2);
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Хозрасчет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
     zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodsocstr,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }
 }
 


}

