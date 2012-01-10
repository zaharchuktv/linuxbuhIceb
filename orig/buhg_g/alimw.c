/* $Id: alimw.c,v 1.11 2011-02-21 07:35:51 sasa Exp $ */
/*22.12.2010    01.12.1992      Белых А.И.     alimw .c
Подпрограмма расчета алиментов
Переменные данные лежат в файле alim.alx
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"
#include        "zarp1.h"

void alim_ps(class iceb_u_double *suma,class iceb_u_double *p_sbor,char *stroka);
double alim_ps_r(class iceb_u_double *suma,class iceb_u_double *p_sbor,double suma_nah);

extern double   okrg;
extern struct ZAR zar;
extern SQL_baza bd;

void alimw(int tn,short mp,short gp,int podr,const char *uder_only,FILE *ff_prot,GtkWidget *wpredok)
{
int kolstr=0;
char            br[512];
short           ku=0;   /*Код начисления*/
short           kps=0;   /*Код почтового сбора*/
double          nah=0.;
double          proc,prsb;
double          bb,bb1;
int            tnn;
int             i,i1;
short		d;
char            knr[512]; /*Коды не входящие в расчет*/
short           shetchik;
SQL_str         row,row_alx;
class SQLCURSOR cur,cur_alx;
char		strsql[512];
char		shetalim[20];
char		shetps[32];
char		koment[512];
char		fio[512];
char		shetban[10]; /*счет на который перечисляются алименты*/
char	alimudvr[512]; //Удержания входящие в расчёт алиментов
short metka_proh=0;
class iceb_u_str imaf("zaralim.alx");

class ZARP     zp;

memset(alimudvr,'\0',sizeof(alimudvr));
sprintf(strsql,"select str from Alx where fil='zaralim.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zaralim.alx\n");
  return;
 }

iceb_u_dpm(&d,&mp,&gp,5);

zp.tabnom=tn;
zp.prn=2;
zp.knu=ku;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.mesn=mp; zp.godn=gp;
zp.nomz=0;
zp.podr=podr;

memset(knr,'\0',sizeof(knr));
prsb=proc=0.;

memset(shetps,'\0',sizeof(shetps));
memset(shetalim,'\0',sizeof(shetalim));


class iceb_u_double suma_p_ua; //диапазоны сумм по Украине
class iceb_u_double suma_p_sng; //диапазоны сумм по СНГ

class iceb_u_double suma_p_sng_ps; //Проценты почтового сбора по СНГ
class iceb_u_double suma_p_ua_ps; //Проценты почтового сбора по Украине

tnn=0;
shetchik=ku=kps=0;
int metka_ps=0;
double nds1=0.;
double min_sum_ua=0.;
double min_sum_sng=0.;
double min_sum_ps=0.;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  memset(br,'\0',sizeof(br));
  if(iceb_u_polen(row_alx[0],br,sizeof(br),1,'|') != 0)
     continue;
  if(iceb_u_SRAV("Код удержания алиментов",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    ku=(short)iceb_u_atof(br);
   
      /*Проверяем есть ли код удержания в списке*/

    sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='2' and \
    knah=%d",tn,ku); 
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) < 1)
     {
      return;
     }
    strncpy(shetalim,row[0],sizeof(shetalim)-1);

      
    nds1=iceb_pnds(1,mp,gp,wpredok);

    if(ff_prot != NULL)
     fprintf(ff_prot,"\nРасчёт алиментов\n\
---------------------------------------------\n\
Код алиментов=%d\n\
НДС=%.2f\n",ku,nds1);
    if(iceb_u_proverka(uder_only,ku,0,0) != 0)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Код %d исключён из расчёта\n",ku);
      return; 
     }
    continue;
   }

  if(iceb_u_SRAV("Код удержания почтового сбора",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    kps=(short)iceb_u_atof(br);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Код почтового сбора=%d\n",kps);
    sprintf(strsql,"select shet from Uder where kod=%d",kps);

    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"%s %d\n",gettext("alim-Не найден код почтового сбора"),kps);
     }
    else
      strncpy(shetps,row[0],sizeof(shetps)-1);
    continue;
   }

  if(iceb_u_SRAV("Коды удержаний входящие в расчет алиментов",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],alimudvr,sizeof(alimudvr),2,'|');
    if(ff_prot != NULL)
     fprintf(ff_prot,"Коды удержаний входящие в расчет алиментов=%s\n",alimudvr);
    continue;
   }
  if(iceb_u_SRAV("Коды начислений не входящие в расчет",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],knr,sizeof(knr),2,'|');
    if(ff_prot != NULL)
     fprintf(ff_prot,"Коды начислений не входящие в расчет=%s\n",knr);
    continue;
   }
  if(iceb_u_SRAV("Почтовый сбор по Украине",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    if(ff_prot != NULL)
     fprintf(ff_prot,"Почтовый сбор по Украине=%s\n",br);
    alim_ps(&suma_p_ua,&suma_p_ua_ps,br);
    continue;
   }
  if(iceb_u_SRAV("Почтовый сбор по СНД",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    if(ff_prot != NULL)
     fprintf(ff_prot,"Почтовый сбор по СНД=%s\n",br);
    alim_ps(&suma_p_sng,&suma_p_sng_ps,br);
    continue;
   }
  if(iceb_u_SRAV("Минимальная сумма почтового сбора по Украине без НДС",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    min_sum_ua=iceb_u_atof(br);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Минимальная сумма почтового сбора по Украине без НДС=%.2f\n",min_sum_ua);
    continue;
   }
  if(iceb_u_SRAV("Минимальная сумма почтового сбора по СНГ без НДС",br,0) == 0)
   {
    iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
    min_sum_sng=iceb_u_atof(br);
    if(ff_prot != NULL)
     fprintf(ff_prot,"Минимальная сумма почтового сбора по СНГ без НДС=%.2f\n",min_sum_sng);
    continue;
   }

  tnn=(long)iceb_u_atof(br);
  if(tnn != tn)
   continue;
  
  if(ff_prot != NULL)
   fprintf(ff_prot,"Табельный номер=%d\n",tnn);
//   printw("Табельный номер=%d\n",tnn);
//   refresh();
  if(metka_proh == 0)
   {
      /*Повторно считаем чтобы учесть обязательные удержания*/
      zapmasw(tn,mp,gp,wpredok);
      nah=0.;
      for(i=0;i<razm;i++)
       {
        
        if(zar.prnu[i] == 0)
         break;

        /*Берем алименты все начисления минус обязательные удержания
        тоесть с суммы выплаченной зарплаты*/

        if(zar.prnu[i] == 1 )
         {
          sprintf(br,"%d",zar.knu[i]);
          if(iceb_u_pole1(knr,br,',',0,&i1) == 0 || iceb_u_SRAV(knr,br,0) == 0)
             continue;
         }

        if(zar.prnu[i] == 2 )
         {
          sprintf(br,"%d",zar.knu[i]);
          if(iceb_u_proverka(alimudvr,br,0,1) != 0)
            continue;
         }
        if(ff_prot != NULL)  
          fprintf(ff_prot,"%d %d %.2f\n",zar.prnu[i],zar.knu[i],zar.sm[i]);
        nah+=zar.sm[i];


       }


      if(nah <=0.)
       {
        return;
       }
   }
  metka_proh++;

  iceb_u_polen(row_alx[0],br,sizeof(br),2,'|');
  proc=iceb_u_atof(br);

  iceb_u_polen(row_alx[0],br,sizeof(br),3,'|');
  metka_ps=atoi(br);
  iceb_u_polen(row_alx[0],fio,sizeof(fio),4,'|');
  iceb_u_polen(row_alx[0],fio,sizeof(fio),4,'|');
  iceb_u_polen(row_alx[0],shetban,sizeof(shetban),6,'|');
  memset(koment,'\0',sizeof(koment));    
  if(shetban[0] != '\0')
    strncpy(koment,shetban,sizeof(koment)-1);    
  else
    strncpy(koment,fio,sizeof(koment)-1);    
  

  bb=nah*proc/100.;
  bb=iceb_u_okrug(bb,okrg)*-1;

  prsb=0.;
  if(metka_ps == 1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Почтовый сбор по ставкам Украины.\n");
    prsb=alim_ps_r(&suma_p_ua,&suma_p_ua_ps,bb);
    min_sum_ps=min_sum_ua;
   }
  if(metka_ps == 2)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Почтовый сбор по ставкам СНГ.\n");
    prsb=alim_ps_r(&suma_p_sng,&suma_p_sng_ps,bb);
    min_sum_ps=min_sum_sng;
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма с которой начисляются алименты=%.2f\n\
Просент алиментов =%.2f\n\
Сумма алиментов=%.2f\n\
Процент почтового сбора=%.2f\n",
   nah,proc,bb,prsb);
  
  strcpy(zp.shet,shetalim);
      
  zp.knu=ku;
  zapzarpw(&zp,bb,d,mp,gp,0,shetalim,"",0,podr,"",wpredok);
//  zapzarp(d,mp,gp,tn,2,ku,bb,shetalim,mp,gp,0,shetchik,koment,podr,"",zp); 

  bb1=bb*prsb/100.; // почтовый сбор с того кто получает
  bb1=iceb_u_okrug(bb1,okrg);


  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма почтового сбора без НДС=%.2f*%.2f/100.=%.2f\n",bb,prsb,bb1);

  if(min_sum_ps != 0. && min_sum_ps > bb1*-1)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Почтовый сбор меньше минимальной суммы почтового сбора %.2f !\n\
Устанавливаем почтовый сбор в размере %.2f\n",min_sum_ps,min_sum_ps);     
    bb1=min_sum_ps*-1;
   }

  double suma_nds=bb1*nds1/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg);

  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма НДС=%.2f*%.2f/100.=%.2f\n",bb1,nds1,suma_nds);

  bb1+=suma_nds;

  if(ff_prot != NULL)
   fprintf(ff_prot,"Сумма почтового сбора с НДС=%.2f\n",bb1);


  if(kps != 0 )
   {
    strcpy(zp.shet,shetps);
    
    zp.knu=kps;
    zapzarpw(&zp,bb1,d,mp,gp,0,shetps,"",0,podr,"",wpredok);
//    zapzarp(d,mp,gp,tn,2,kps,bb1,shetps,mp,gp,0,shetchik,koment,podr,"",zp); 
   }
  shetchik++;
  zp.nomz=shetchik;
 }

}
/**********************************/
/*Расшифровка строки с процентами почтового сбора*/
/***********************************************/
void alim_ps(class iceb_u_double *suma,class iceb_u_double *p_sbor,char *stroka)
{
int kolpol=iceb_u_pole2(stroka,',');
if(kolpol == 0)
 return;
char bros[512];
char bros1[100];
for(int i=0; i < kolpol;i++)
 {
  iceb_u_polen(stroka,bros,sizeof(bros),i+1,',');

  iceb_u_polen(bros,bros1,sizeof(bros1),1,'/');
  suma->plus(atof(bros1),-1);  

  iceb_u_polen(bros,bros1,sizeof(bros1),2,'/');
  p_sbor->plus(atof(bros1),-1);
 }
}
/*************************/
/*Расчёт почтового сбора*/
/**************************/
double alim_ps_r(class iceb_u_double *suma,class iceb_u_double *p_sbor,double suma_nah)
{
double ps=0.;
for(int i=0 ; i < suma->kolih() ; i++)
 {
  if(suma_nah <= suma->ravno(i) || suma->ravno(i) == 0.)
   {
    ps=p_sbor->ravno(i);
    break;
   }
 }

return(ps);
}
