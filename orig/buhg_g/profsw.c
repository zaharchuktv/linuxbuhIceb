/* $Id: profsw.c,v 1.8 2011-04-14 16:09:36 sasa Exp $ */
/*22.12.2010    02.12.1992      Белых А.И.      profsw.c
Подпрограмма расчета отчисления в профсоюзный фонд
*/
#include        "buhg_g.h"

extern short	metkarnb; //0-расчет налогов для бюджета пропорциональный 1-последовательный
extern double   okrg;
extern char	*shetb; /*Бюджетные счета*/
extern char	*shetps; /*Счета профсоюзного сбора*/
extern short	kuprof; //Код удержания в профсоюзный фонд
extern float	procprof; //Процент отчисления в профсоюзный фонд
extern short	*knvrprof; //Коды не входящие в расчет удержаний в профсоюзный фонд
extern short    *kodmp;   /*Коды материальной помощи*/

void profsw(int tn,short mp,short gp,int podr,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok)
{
char            bros[512];
double          nah,nah1,bb2;
short           d;
char		kom[2];
char		shet[32];
SQL_str         row;
char		strsql[512];
double		sumanb,sumabu;
class ZARP     zp;
double		matpomb=0.;
double          suma_zarp_o; //Сумма чистой зарплаты общая
double          suma_zarp_b; //Сумма чистой зарплаты бюджетная
double suma_boln_rm=0.;
SQLCURSOR curr;
/*
printw("profs---------------\n");
refresh();
*/
if(kuprof == 0)
  return;

iceb_u_dpm(&d,&mp,&gp,5);
zp.tabnom=tn;
zp.prn=2;
zp.knu=kuprof;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;
  
/*Проверяем есть ли код удержания в списке*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
knah=%d",tn,kuprof); 
//printw("%s\n",strsql);
if(iceb_sql_readkey(strsql,&row,&curr,wpredok) > 0)
 {
  
  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"\nРасчёт профсоюзного взноса\n\
-----------------------------------------------\n");  
    if(knvrprof != NULL)
     {
      fprintf(ff_prot,"Коды невходящие в расчет:");
     
      for(int kkk=1; kkk < knvrprof[0] ; kkk ++)
        fprintf(ff_prot,"%d ",knvrprof[kkk]);
      fprintf(ff_prot,"\n");
     }
   }

  class zar_read_tn1h nastr;
  zar_read_tn1w(1,mp,gp,&nastr,ff_prot,wpredok);

  if(iceb_u_proverka(uder_only,kuprof,0,0) != 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код %d исключён из расчёта\n",kuprof);
    return;
   }
   
  strncpy(shet,row[0],sizeof(shet)-1);
  double suma_nfnz,suma_nfnz_b;
  nah=findnahmw(mp,3,knvrprof,ff_prot,&sumanb,&sumabu,&suma_nfnz,&suma_nfnz_b,&suma_zarp_o,&suma_zarp_b,&suma_boln_rm);
  if(ff_prot != NULL)
     fprintf(ff_prot,"Берем в расчет %.2f\n",nah);
//  nah-=matpom1(tn,mp,gp,kodmp,gettext("Матеpиальная помощь"),ff_prot,0,&matpomb,0);

  if(nah > nastr.max_sum_for_soc)
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"Превышение максимума %.2f > %.2f\n",nah,nastr.max_sum_for_soc);
    nah=nastr.max_sum_for_soc;      
   }
  

  nah1=nah*procprof/100.*(-1);
  nah1=iceb_u_okrug(nah1,okrg);
  if(ff_prot != NULL)
     fprintf(ff_prot,"%.2f*%.2f/100=%.2f\n",nah,procprof,nah*procprof/100.);

  kom[0]='\0';
  if(shetb == NULL || shetps == NULL)
   {
    strcpy(zp.shet,shet);
    zapzarpw(&zp,nah1,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//    zapzarp(d,mp,gp,tn,2,kuprof,nah1,shet,mp,gp,0,0,kom,podr,"",zp); 
   }
  else
   {
    bb2=0.;
    if(metkarnb == 1)
     {
      if(sumabu > nastr.max_sum_for_soc)
       {
        if(ff_prot != NULL)
           fprintf(ff_prot,"Превышение максимума суммы для бюджета %.2f > %.2f\n",sumabu,nastr.max_sum_for_soc);
        sumabu=nastr.max_sum_for_soc;
       }

      bb2=(sumabu-matpomb)*procprof/100.*(-1);
      if(ff_prot != NULL)
        fprintf(ff_prot,"Бюджетный профсоюз (%.2f-%.2f)*%f/100.*-1=%.2f",
        sumabu,matpomb,procprof,bb2);
     }
    if(metkarnb == 0)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Разбивка суммы:%.2f не бюджет=%.2f бюджет:%.2f\n",
       nah,sumanb,sumabu);
      if(sumabu > 0.009)
       {
        //Если хозрасчет не равен нолю то долю определять надо
        if(sumanb != 0.)
         bb2=nah1*sumabu/nah;
        else
         bb2=nah1;
       }
     }

    bb2=iceb_u_okrug(bb2,okrg);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Налог бюджет=%.2f\n",bb2);
    iceb_u_polen(shetps,shet,sizeof(shet),2,',');
    if(shet[0] != '\0')
     {
      strcpy(bros,gettext("Бюджет"));
      strcpy(zp.shet,shet);
      zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kuprof,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
     }

    iceb_u_polen(shetps,shet,sizeof(shet),1,',');
    bb2=nah1-bb2;
    bb2=iceb_u_okrug(bb2,okrg);
    if(shet[0] != '\0')
     {
      strcpy(bros,gettext("Небюджет"));
      strcpy(zp.shet,shet);
      zapzarpw(&zp,bb2,d,mp,gp,0,shet,"",0,podr,"",wpredok);
//      zapzarp(d,mp,gp,tn,2,kuprof,bb2,shet,mp,gp,0,0,bros,podr,"",zp);
     }
   }
 }

}





