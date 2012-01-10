/*$Id: ras_podohw.c,v 1.8 2011-08-29 07:13:43 sasa Exp $*/
/*22.12.2010	17.02.2005	Белых А.И.	ras_podohw.c
Расчёт подоходного налога
*/
#include        "buhg_g.h"

extern char	*shetb; /*Бюджетные счета*/
extern double   okrg; /*Округление*/
extern short    *kodmp;   /*Коды благотворит. помощи*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern char	*shpnb;   /*Бюджетный счет подоходного налога*/
extern short    kodpn;   /*Код подоходного налога*/
extern short	metkarnb; //0-Расчет налогов для бюджета пропорциональный 1-последовательный
extern SQL_baza bd;

double zarsbslw(int tabnom,short mp,short gp,double *suma_bsl_b,double *suma_bsl_start,double *suma_bsl_start_b,int metka_ras,FILE *ff_prot);

double ras_podohw(int  tn, //Табельный номер*/
short mp,short gp,
int podr,
double fbezrab,
double fbezrabb,
double sumapo,
double sumapob,
double sumaos,
double sumaosb,
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
class ZARP     zp;
short		d;

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=kodpn;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nРозсчёт прибуткового податку.\n\
---------------------------------------------------\n");
 }

if(iceb_u_proverka(uder_only,kodpn,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodpn);
  return(0.);    
 }
double suma_ndfv=0.; //Сумма неденежных форм расчетов
double sumanb=0.;
double sumabu=0.;
double suma_nah=0.;

if(metka_ras == 0)
  suma_nah=find_podohw(mp,ff_prot,&suma_ndfv,&sumanb,&sumabu);
else
 {
  suma_nah=find_podohw(mp,gp,tn,ff_prot,&suma_ndfv,&sumanb,&sumabu,wpredok);
  //если есть сyмма больничного значит делаем перерасчёт подоходного для больничных на которые 
  //распространяется социальная льгота 
  if(suma_boln != 0.)
   if(ff_prot != NULL)
    fprintf(ff_prot,"Увеличиваем на сумму больничного\n");
  suma_nah+=suma_boln;
  sumanb+=suma_boln;
 }
double suma_blag_pom,suma_blag_pom_b;
double matpomb=0.;
double nmatpom=matpom1w(tn,mp,gp,kodmp,ff_prot,0,&matpomb,1,&suma_blag_pom,&suma_blag_pom_b,wpredok);

//Нужно помнить что все налоги отрицательные поэтому мы их просто прибавляем
double suma_soco=fbezrab+sumapo+sumaos;
double suma_soco_b=fbezrabb+sumapob+sumaosb;
if(ff_prot != NULL)
  fprintf(ff_prot,"Сумма всех соцотчислений которые надо отнять Безр.=%.2f + Пенс.=%.2f + Соц.=%.2f = %.2f\n",
  fbezrab,sumapo,sumaos,suma_soco);
 

double nah;
double bb;

bb=nah=suma_nah;



if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Перед podohrs - nah=%f bb=%f\n",nah,bb);
 }

double suma_bsl_b=0.;
double suma_bsl_bnds_b=0.;
double suma_bsl_bnds=0.;
double suma_bsl=zarsbslw(tn,mp,gp,&suma_bsl_b,&suma_bsl_bnds,&suma_bsl_bnds_b,metka_ras,ff_prot);
//double suma_bsl=prosmasw(kn_bsl,1,mp,&suma_bsl_b);

if(ff_prot != NULL)
 {
  fprintf(ff_prot,"Сумма начислений на которые не распространяется социальная льгота:%.2f/%.2f\n",
  suma_bsl,suma_bsl_b);
//  fprintf(ff_prot,"%.2f-%.2f=%.2f\n",nah,suma_bsl,nah-suma_bsl);

 }     
double suma_obl_blag_pom=suma_blag_pom-nmatpom;
if(suma_obl_blag_pom < 0)
 suma_obl_blag_pom=0.;

double suma_obl_blag_pom_b=suma_blag_pom_b-matpomb;
if(suma_obl_blag_pom_b < 0)
 suma_obl_blag_pom_b=0.;
 

double suma_s_soc_lg=0.;
double suma_s_soc_lg_b=0.;

//suma_s_soc_lg=suma_nah-suma_bsl-suma_blag_pom;
//suma_s_soc_lg_b=sumabu-suma_bsl_b-suma_blag_pom_b;

suma_s_soc_lg=suma_nah-suma_bsl_bnds-suma_bsl;
suma_s_soc_lg_b=sumabu-suma_bsl_bnds_b-suma_bsl_b;


nah=podohrs1w(tn,mp,gp,suma_s_soc_lg,suma_soco,suma_bsl,suma_ndfv,ff_prot,wpredok);


/*Проверяем есть ли код удержания подоходного налога в списке*/

sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,kodpn);
/*
printw("%s\n",strsql);
refresh();
*/
char bros[312];
if(sql_readkey(&bd,strsql,&row,&curr) <= 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не введено удержание подоходного налога !\n");
  return(0.);
 }

strncpy(shet,row[0],sizeof(shet)-1);
memset(bros,'\0',sizeof(bros));
if(shetb == NULL || shpnb == NULL)
 {
  strcpy(zp.shet,shet);
  if(metka_zap == 0)
   zapzarpw(&zp,nah,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//   zapzarp(d,mp,gp,tn,2,kodpn,nah,shet,mp,gp,0,0,bros,podr,"",zp);
 }
else
 {

  
  double bb2=0.;
  if(metkarnb == 1) //Последовательный метод расчета налогов
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"\nРасчёт подоходного для бюджетных начислентий.\n");
      
      fprintf(ff_prot,"Сумма всех соцотчислений которые надо отнять Безр.=%.2f + Пенс.=%.2f + Соц.=%.2f = %.2f\n",
      fbezrabb,sumapob,sumaosb,suma_soco_b);
     }
    bb2=podohrs1w(tn,mp,gp,suma_s_soc_lg_b,suma_soco_b,suma_bsl_b,0.,ff_prot,wpredok);

    if(ff_prot != NULL)
      fprintf(ff_prot,"Бюджет последовательный:%.2f Счет:%s\n",bb2,shpnb);
   } 

  if(metkarnb == 0) //Пропорциональный метод расчета налогов
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Пропорционально:Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f\n",
     bb,sumanb,sumabu);
    if(sumabu != 0.)
     {
      
      if(bb != 0.)
       {
        //Если хозрасчет не равен нолю то долю определять надо
        if(sumanb != 0.)
         bb2=nah*sumabu/bb;
        else
         bb2=nah;
       }
     }
   }

  bb2=iceb_u_okrug(bb2,okrg);
  iceb_u_polen(shpnb,shet,sizeof(shet),2,',');
  if(ff_prot != NULL)
    fprintf(ff_prot,"Бюджет:%.2f Счет:%s\n",bb2,shpnb);
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Бюджет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
     zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodpn,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }

  iceb_u_polen(shpnb,shet,sizeof(shet),1,',');
  bb2=nah-bb2;
  bb2=iceb_u_okrug(bb2,okrg);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Не бюджет:%.2f(%.2f) Счет:%s\n",bb2,nah,shet);
  if(shet[0] != '\0')
   {
    strcpy(bros,gettext("Хозрасчет"));
    strcpy(zp.shet,shet);
    if(metka_zap == 0)
     zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kodpn,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
   }
  
 }


return(nah);
}
