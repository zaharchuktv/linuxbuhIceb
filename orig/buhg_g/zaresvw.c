/*$Id: zaresvw.c,v 1.6 2011-06-07 08:52:27 sasa Exp $*/
/*16.05.2011 	15.01.2011	Белых А.И.	zaresvw.c
Расчёт удержания единого социального взноса с человека

*/
#include <stdlib.h>
#include <math.h>
#include "buhg_g.h"


extern char *shetb; /*Бюджетные счета*/
extern int kod_esv; /*Код удержания единого социального взноса*/
extern int kod_esv_bol; /*Код удержания единого социального взноса*/
extern int kod_esv_inv; /*Код удержания единого социального взноса*/
extern int kod_esv_dog; /*Код удержания единого социального взноса*/
extern int kod_esv_vs;

extern class iceb_u_str knvr_esv_r; /*Коды не входящие в расчёт единого социального взноса с работника*/
extern class iceb_u_str kod_zv_gr; /*Коды званий гражданских*/
extern  short   *kodbl; /*Код больничного*/
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу

extern SQL_baza bd;
double proc_esv_prim=0.; /*применённый процент единого социального взноса*/

double zaresvw(int tabn,short mp,short gp,int podr,
double *esv_s_lgot,
double *esv_s_lgotb,
int metka_zap, /*0-делать запись 1-нет*/
double suma_boln_for_per, /*используется только для перерасчёта подоходного налога из-за больничного*/
const char *uder_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
double suma=0.;
double suma_ob=0.;
double suma_bud=0.;
double suma_bol_ob=0.;
double suma_bol_bud=0.;
double suma_s_lgot=0.;
double suma_s_lgotb=0.;
double suma_s_lgot_bol=0.;
double suma_s_lgotb_bol=0.;
class iceb_u_str shet("");
int kolstr=0;
int knah=0;

if(ff_prot != NULL)
  fprintf(ff_prot,"Расчёт единого социального взноса\n\
------------------------------------------------------------\n");

/*Проверяем есть ли код удержания взноса*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabn,kod_esv);
if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
 {
  sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabn,kod_esv_inv);
  if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
   {
    sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabn,kod_esv_bol);
    if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
     {
      sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabn,kod_esv_dog);
      if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
       {
        sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and knah=%d",tabn,kod_esv_vs);
        if(sql_readkey(&bd,strsql,&row,&cur) <= 0)
         {

          if(ff_prot != NULL)
           fprintf(ff_prot,"\nНе введен код единого социального взноса !\n");
          return(0.);
         }
       }
     }
   }
 }
shet.new_plus(row[0]);

if(iceb_u_proverka(uder_only,kod_esv,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kod_esv);
  return(0.); 
 }

 
/*Смотрим начисления*/
short mesn=0;
sprintf(strsql,"select knah,suma,shet,mesn from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d and prn='1' and suma <> 0.",gp,mp,1,gp,mp,31,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
  if(iceb_u_proverka(knvr_esv_r.ravno(),row[0],0,1) == 0)
   continue;   
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %s %s %s\n",row[0],row[1],row[2],row[3]);

  knah=atoi(row[0]);
  suma=atof(row[1]);
  mesn=atoi(row[3]);
  if(provkodw(kodbl,knah) >= 0)
    suma_bol_ob+=suma;
  else
    suma_ob+=suma;


  if(iceb_u_proverka(shetb,row[2],0,1) == 0)
   {
    if(provkodw(kodbl,knah) >= 0)
      suma_bol_bud+=suma;
    else
      suma_bud+=suma;
   }

  if(provkodw(kn_bsl,knah) < 0)
   {
    if(provkodw(kodbl,knah) >= 0)
     { 
      if(mesn == mp)
       suma_s_lgot_bol+=suma;
     }
    else      
      suma_s_lgot+=suma;

    if(iceb_u_proverka(shetb,row[2],0,1) == 0)
     {
      if(provkodw(kodbl,knah) >= 0)
       {
        if(mesn == mp)
          suma_s_lgotb_bol+=suma;
       }
      else
        suma_s_lgotb+=suma;
     }
   }

 }

return(zaresv_rs(mp,gp,tabn,podr,suma_ob,suma_bud,suma_bol_ob,suma_bol_bud,suma_s_lgot,suma_s_lgotb,suma_s_lgot_bol,suma_s_lgotb_bol,suma_boln_for_per,esv_s_lgot,esv_s_lgotb,metka_zap,shet.ravno(),ff_prot,wpredok));

}



/*************************************/
/***********************************/

void zaresv_ras(double suma,float proc,double *suma_esv,float max_sum_for_soc,FILE *ff_prot)
{
if(suma > max_sum_for_soc)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма %.2f > %.2f\nВ расчёт берём %.2f\n",suma,max_sum_for_soc,max_sum_for_soc);
  suma=max_sum_for_soc;
 }
*suma_esv=0.;
if(suma > 0.)
 {
  *suma_esv=suma*proc/100.*-1;

  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма единого социального взноса %.2f*%.2f/100.=%.2f\n",suma,proc,*suma_esv);
 }


}
/****************************************/
/*Расчёт*/
/***************************************/
double zaresv_rs(short mp,short gp,
int tabn,
int podr,
double suma_ob,   /*Cумма для расчёта общего единого социального взноса с не больничных*/
double suma_bud,
double suma_bol_ob, /*Cумма больничных для расчёта единого социального взноса*/
double suma_bol_bud,
double suma_s_lgot,  /*Сумма для которой применяется социальная льгота для расчёта е.с.в. с не больничных*/
double suma_s_lgotb,
double suma_s_lgot_bol, /*Сумма больничных для которых применяется социальная льгота*/
double suma_s_lgotb_bol,
double suma_boln_for_per,  /*Сумма больничных для перерасчёта подоходного налога - обычно ноль*/
double *esv_s_lgot,  /*Посчитанная сумма е.с.в. с суммы начислений для которых применялась соц.льгота */
double *esv_s_lgotb, /*Посчитанная сумма е.с.в. c бюджетной суммы*/
int metka_zap,  /*0- делать запись 1-нет*/
const char *shetvud, /*Счёт в удержании -нужен только для записи*/
FILE *ff_prot,
GtkWidget *wpredok)
{
int kod_esv_vr=kod_esv;
char bros[1024];
struct ZARP     zp;
double suma_esv=0.;
double suma_esvb=0.;
double suma_esv_bol=0.;
double suma_esv_bol_bud=0.;
double suma_esv_ob=0.;
class iceb_u_str shet("");
class iceb_u_str shet_bud("");
SQL_str row;
class SQLCURSOR cur;
char strsql[1024];
short d;
class zarrnesvp_rek esvp;

zarrnesvpw(mp,gp,&esvp,ff_prot,wpredok);

proc_esv_prim=esvp.proc_esv;

if(ff_prot != NULL)
  fprintf(ff_prot,"Коды не входящие в расчёт-%s\n\
Проценты %f С больничного %f С инвалида %f С договоров %f С военных %f\n",
knvr_esv_r.ravno(),
esvp.proc_esv,
esvp.proc_esv_bol,
esvp.proc_esv_inv,
esvp.proc_esv_dog,
esvp.proc_esv_vs);



iceb_u_dpm(&d,&mp,&gp,5);
/***********
zp.dz=d;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
*************/
zp.tabnom=tabn;
zp.prn=2;
zp.knu=kod_esv;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

/*Читаем бюджетный счёт*/
sprintf(strsql,"select shetb from Uder where kod=%d",kod_esv_vr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 shet_bud.new_plus(row[0]);

if(ff_prot != NULL)
 fprintf(ff_prot,"Cумма общая %.2f\nСумма бюджетная %.2f Cумма больничных %.2f Сумма бюджетных больничных %.2f\n",
 suma_ob,suma_bud,suma_bol_ob,suma_bol_bud);

 
int metka_inv=zarprtnw(mp,gp,tabn,"zarinv.alx",ff_prot,wpredok); 
if(metka_inv == 1)
 {
  kod_esv_vr=kod_esv_inv;
  proc_esv_prim=esvp.proc_esv_inv;
  if(ff_prot != NULL)
   fprintf(ff_prot,"Инвалид ставка %.2f\n",proc_esv_prim);

  /*Читаем счёт*/
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_esv_vr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    shet.new_plus(row[0]);
    shet_bud.new_plus(row[1]);
   }
 }

int metka_dog=zarprtnw(mp,gp,tabn,"zardog.alx",ff_prot,wpredok); 
if(metka_dog == 1)
 {
  kod_esv_vr=kod_esv_dog;
  proc_esv_prim=esvp.proc_esv_dog;
  if(ff_prot != NULL)
   fprintf(ff_prot,"Работает по договору ставка %.2f\n",proc_esv_prim);
  /*Читаем счёт*/
  sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_esv_vr);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    shet.new_plus(row[0]);
    shet_bud.new_plus(row[1]);
   }
 }
sprintf(strsql,"select zvan from Zarn where tabn=%d and god=%d and mes=%d",tabn,gp,mp);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
if(atoi(row[0]) != 0)
 if(iceb_u_proverka(kod_zv_gr.ravno(),row[0],0,0) != 0)
  {

   if(ff_prot != NULL)
    fprintf(ff_prot,"Военный %s != %s\n",kod_zv_gr.ravno(),row[0]);
   if(kod_esv_vs == 0)
    {
     if(ff_prot != NULL)
      fprintf(ff_prot,"Код удержания единого социального взноса для военных равен нолю!\n");
     return(0.); 
    }
   kod_esv_vr=kod_esv_vs;
   proc_esv_prim=esvp.proc_esv_vs;
   /*Читаем счёт*/
   sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_esv_vr);
   if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    {
     shet.new_plus(row[0]);
     shet_bud.new_plus(row[1]);
    }
  }

class iceb_u_str shet_bol("");
class iceb_u_str shet_bolb("");
/*Читаем счёт*/
sprintf(strsql,"select shet,shetb from Uder where kod=%d",kod_esv_bol);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  shet_bol.new_plus(row[0]);
  shet_bolb.new_plus(row[1]);
 }


if(ff_prot != NULL)
 fprintf(ff_prot,"Только с суммы на которую распространяется льгота\n");

class zar_read_tn1h nastr;
zar_read_tn1w(1,mp,gp,&nastr,ff_prot,wpredok);


double bb1=0.,bb2=0.;
zaresv_ras(suma_s_lgot,proc_esv_prim,&bb1,nastr.max_sum_for_soc,ff_prot);
if(suma_boln_for_per != 0.)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Увеличиваем сумму больничного на который распространяется соц. льгота %.2f+%.2f=%.2f\n",suma_s_lgot_bol,suma_boln_for_per,suma_s_lgot_bol+suma_boln_for_per);
  suma_s_lgot_bol+=suma_boln_for_per;
 }
zaresv_ras(suma_s_lgot_bol,esvp.proc_esv_bol,&bb2,nastr.max_sum_for_soc,ff_prot);

*esv_s_lgot=bb1+bb2;
if(ff_prot != NULL)
 fprintf(ff_prot,"%.2f+%.2f=%.2f\n",bb1,bb2,*esv_s_lgot);

if(ff_prot != NULL)
 fprintf(ff_prot,"C cуммы не больничных\n");
zaresv_ras(suma_ob,proc_esv_prim,&suma_esv,nastr.max_sum_for_soc,ff_prot);
if(suma_boln_for_per != 0.)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Увеличиваем общюю сумму больничного %.2f+%.2f=%.2f\n",suma_bol_ob,suma_boln_for_per,suma_bol_ob+suma_boln_for_per);
  suma_bol_ob+=suma_boln_for_per;
 }
if(suma_bol_ob != 0.)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"C больничного\n");
  zaresv_ras(suma_bol_ob,esvp.proc_esv_bol,&suma_esv_bol,nastr.max_sum_for_soc,ff_prot);

 }
if(suma_bol_bud != 0.)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"C бюджетного больничного\n");
  zaresv_ras(suma_bol_bud,esvp.proc_esv_bol,&suma_esv_bol_bud,nastr.max_sum_for_soc,ff_prot);

 }

suma_esv_ob=suma_esv+suma_esv_bol;

if(ff_prot != NULL)
 fprintf(ff_prot,"Общая сумма=%.2f+%.2f=%.2f\n",suma_esv,suma_esv_bol,suma_esv_ob);

if(shetb == NULL || shet_bud.getdlinna() <= 1)
 {
  
  
  if(metka_zap == 0)
   {
      strcpy(zp.shet,shet.ravno());
      if(ff_prot != NULL)
       fprintf(ff_prot,"1пишем код %d\n",kod_esv_vr);
      sprintf(bros,"ЄСВ-%.2f%%",proc_esv_prim);
//      zapzarp(d,mp,gp,tabn,2,kod_esv_vr,suma_esv,shet.ravno(),mp,gp,0,0,bros,podr,"",zp);
      zp.knu=kod_esv_vr;
      zapzarpw(&zp,suma_esv,d,mp,gp,0,shet.ravno(),bros,0,podr,"",wpredok);

      if(ff_prot != NULL)
       fprintf(ff_prot,"2пишем код %d\n",kod_esv_bol);
      strcpy(zp.shet,shet_bol.ravno());
  
      sprintf(bros,"ЄСВ-%.2f%%",esvp.proc_esv_bol);
//      zapzarp(d,mp,gp,tabn,2,kod_esv_bol,suma_esv_bol,shet_bol.ravno(),mp,gp,0,0,bros,podr,"",zp);
      zp.knu=kod_esv_bol;
      zapzarpw(&zp,suma_esv_bol,d,mp,gp,0,shet_bol.ravno(),bros,0,podr,"",wpredok);
   }
 }
else
 {
  /*Бюджетный больничный*/
  if(shet_bolb.getdlinna() > 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"3 пишем код %d %.2f %d.%d\n",kod_esv_bol,suma_esv_bol_bud,mp,gp);
     strcpy(zp.shet,shet_bolb.ravno());

    sprintf(bros,"ЄСВ-%.2f%%",esvp.proc_esv_bol);

    zp.knu=kod_esv_bol;
    if(metka_zap == 0)
      zapzarpw(&zp,suma_esv_bol,d,mp,gp,0,shet_bol.ravno(),bros,0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tabn,2,kod_esv_bol,suma_esv_bol_bud,shet_bolb.ravno(),mp,gp,0,0,bros,podr,"",zp,ff_prot);
   } 
  /*Хозрасчётный больничный*/
  if(ff_prot != NULL)
   fprintf(ff_prot,"4 пишем код %d %.2f %s\n",kod_esv_bol,suma_esv_bol-suma_esv_bol_bud,shet_bol.ravno());
   strcpy(zp.shet,shet_bol.ravno());

  sprintf(bros,"ЄСВ-%.2f%%",esvp.proc_esv_bol);
  zp.knu=kod_esv_bol;
  if(metka_zap == 0)
    zapzarpw(&zp,suma_esv_bol-suma_esv_bol_bud,d,mp,gp,0,shet_bol.ravno(),bros,0,podr,"",wpredok);
//    zapzarp(d,mp,gp,tabn,2,kod_esv_bol,suma_esv_bol-suma_esv_bol_bud,shet_bol.ravno(),mp,gp,0,0,bros,podr,"",zp,ff_prot);

  /*Общюю сумму мы уже знаем.Делаем расчёт для бюджета и отнимаем от общей суммы*/
  if(ff_prot != NULL)
   fprintf(ff_prot,"Делаем расчёт для бюджета\n");

  if(ff_prot != NULL)
   fprintf(ff_prot,"Только зарплата\n");
  
  zaresv_ras(suma_s_lgotb,proc_esv_prim,esv_s_lgotb,nastr.max_sum_for_soc,ff_prot);
  
  if(ff_prot != NULL)
   fprintf(ff_prot,"Cо всей суммы\n");

  zaresv_ras(suma_bud,proc_esv_prim,&suma_esvb,nastr.max_sum_for_soc,ff_prot);
  
  
  double suma_zap=suma_esvb;
  if(ff_prot != NULL)
    fprintf(ff_prot,"Бюджетная сумма=%.2f\n",suma_zap);
  if(shet.getdlinna() > 1)
   {
    sprintf(bros,"%s-ЄСВ-%.2f%%",gettext("Бюджет"),proc_esv_prim);
    strcpy(zp.shet,shet_bud.ravno());
    if(metka_zap == 0 )
     {
//      zapzarp(d,mp,gp,tabn,2,kod_esv_vr,suma_zap,shet_bud.ravno(),mp,gp,0,0,bros,podr,"",zp,ff_prot);
      zp.knu=kod_esv_vr;
      zapzarpw(&zp,suma_zap,d,mp,gp,0,shet_bud.ravno(),bros,0,podr,"",wpredok);
     }
   }

  if(ff_prot != NULL)
    fprintf(ff_prot,"Хозрасчётная сумма=%.2f-%.2f=%.2f\n",suma_esv,suma_zap,suma_esv-suma_zap);

  suma_zap=suma_esv-suma_zap;


  if(shet.getdlinna() > 1)
   {
    sprintf(bros,"%s-ЄСВ-%.2f%%",gettext("Хозрасчёт"),proc_esv_prim);
    strcpy(zp.shet,shet.ravno());
    if(metka_zap == 0 )
     {
//      zapzarp(d,mp,gp,tabn,2,kod_esv_vr,suma_zap,shet.ravno(),mp,gp,0,0,bros,podr,"",zp,ff_prot);
      zp.knu=kod_esv_vr;
      zapzarpw(&zp,suma_zap,d,mp,gp,0,shet.ravno(),bros,0,podr,"",wpredok);
     }
   }
 }
return(suma_esv_ob);
}
