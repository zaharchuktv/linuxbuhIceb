/* $Id: zarindexw.c,v 1.9 2011-04-06 09:49:17 sasa Exp $ */
/*22.12.2010	19.05.2000	Белых А.И.	zarindexw.c
Расчет индексации зарплаты
*/
#include <stdlib.h>
#include        <errno.h>
#include        "buhg_g.h"
#include "read_nast_indw.h"


extern class index_ua tabl_ind; /*Таблица для индексации зарплаты*/

extern short	kodindex;  /*Код индексации*/
extern short    *kodnvrindex; //Коды не входящие в расчет индексации
extern double   okrg; /*Округление*/
extern short    ggg,mmm;
extern SQL_baza bd;

void zarindexw(int tn, //Табельный номер*/
short mp,short gp,int podr,
const char *nah_only,
FILE *ff_prot,
GtkWidget *wpredok)
{
int		i;
char            strsql[512];
char            bros[512];
short		mi,gi; /*месяц индексации*/
double		procind; /*процент индексации*/
double		sumaog=0.;  /*Сумма ограничения индексации*/
double		suman;   /*Сумма начисленная в месяце индексации*/
double		sumain;  /*Сумма индексации*/
char		shet[56];
short		d;
long		kolstr;
SQL_str         row;
SQLCURSOR cur;
class ZARP     zp;
double		insum=0.; //Индексируемая сумма
//int		kolmes=0;


//printw("zarindex-%d %d.%d %d\n",tn,mp,gp,podr);
//OSTANOV();


if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\n%s-Расчет индексации.\n\
-----------------------------------------------\n\
Коды не входящие в расчет индексации:",__FUNCTION__);
  if(kodnvrindex != NULL)
   for(i=1; i <= kodnvrindex[0]; i++)
    fprintf(ff_prot,"%d ",kodnvrindex[i]);
  fprintf(ff_prot,"\n");
 }

if(kodindex == 0)
 {
  if(ff_prot != 0)
   fprintf(ff_prot,"Код индексации равен нолю!\n");
  return;
 }

if(iceb_u_proverka(nah_only,kodindex,0,0) != 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Код %d исключён из расчёта\n",kodindex);
  return;
 }

/*Проверяем есть ли индексация в списке начислений*/
sprintf(strsql,"select shet from Zarn1 where tabn=%d and prn='1' and \
knah=%d",tn,kodindex);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
  return;

strncpy(shet,row[0],sizeof(shet)-1);
//Читаем дату приёма на работу и дату последнего повышения зарплаты
sprintf(strsql,"select datn,datk,dppz from Kartb where tabn=%d",tn);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдена карточка для табельного номера %d\n",tn);
  return;
 }


short dppz=0,mppz=0,gppz=0; //Дата последнего повышения зарплаты
short dnr=0,mnr=0,gnr=0;    //Дата начала работы
short mb=0,gb=0;       //базовый месяц для расчёта индексации

short du=0,mu=0,gu=0;
iceb_u_rsdat(&du,&mu,&gu,row[1],2);

if(iceb_u_rsdat(&dppz,&mppz,&gppz,row[2],2) == 0)
  if(ff_prot != NULL)
   fprintf(ff_prot,"Дата последнего повышения зарплаты %d.%d\n",mppz,gppz);

if( iceb_u_rsdat(&dnr,&mnr,&gnr,row[0],2) == 0)
  if(ff_prot != NULL)
   fprintf(ff_prot,"Дата принятия на работу %d.%d.%d\n",dnr,mnr,gnr);
 
//class index_ua data;

//read_nast_ind(mp,gp,&data,wpredok);

d=1;
iceb_u_dpm(&d,&mp,&gp,5);
zp.tabnom=tn;
zp.prn=1;
zp.knu=kodindex;
zp.dz=d;
zp.mz=mp;
zp.gz=gp;
zp.godn=gp;
zp.mesn=mp;
zp.nomz=0;
zp.podr=podr;
class zar_read_tn1h nast_mz;

int kolih_mr=tabl_ind.mr.kolih();
if(kolih_mr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдено ни одного месяца расчёта индексации !\n");
  return;
 }

for(int kol=0; kol < kolih_mr ; kol++)
 {
  mi=tabl_ind.mr.ravno(kol);
  gi=tabl_ind.gr.ravno(kol);

  if(ff_prot != NULL)
      fprintf(ff_prot,"\nДата индексации:%d.%d Максимальная сумма индексации:%.2f\n",mi,gi,sumaog);


  if(gppz != 0)
   {
    if(iceb_u_sravmydat(1,mppz,gppz,1,mi,gi) > 0)
     {
      mb=mnr; gb=gnr;
     }
    else
     {
      mb=mppz; gb=gppz;
     }
   }
  else
   {
    mb=mnr; gb=gnr;
   }  
  /*читаем прожиточный минимум*/
  zar_read_tn1w(1,mi,gi,&nast_mz,ff_prot,wpredok);
  sumaog=nast_mz.prog_min_tek;
  
  if(sumaog == 0.)
    continue;
  int kol_mes_tabl=tabl_ind.mi.kolih();
  if(ff_prot != NULL)
   fprintf(ff_prot,"Количество месяцев в таблице=%d\n",kol_mes_tabl);

  procind=1.;

  short dpr,mpr,gpr;

  dpr=1; mpr=mi; gpr=gi;
  iceb_u_dpm(&dpr,&mpr,&gpr,4);    
  float index=0;
  float pred_index=1;

  if(iceb_u_sravmydat(1,mu,gu,1,mi,gi) == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Уволен %d.%d.%d \n",du,mu,gu);
   }
  else  
  for(int kol_tabl=0; kol_tabl < kol_mes_tabl ; kol_tabl++)
   {
/**************
    if(ff_prot != NULL)
     fprintf(ff_prot,"mi.gi=%d.%d mppz.gppz=%d.%d tabl_ind.mi,tabl_ind.gi=%d.%d\n",
     mi,gi,mppz,gppz,tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl));
****************/     
    if(iceb_u_sravmydat(1,mb,gb,1,tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl)) < 0)
    if(iceb_u_sravmydat(1,mpr,gpr,1,tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl)) > 0)
     {

      index=tabl_ind.koef.ravno(kol_tabl)/100.;
//      index=iceb_u_okrug(index,0.001);

      if(ff_prot != NULL)
        fprintf(ff_prot,"%02d.%04d %8.2f %f * %f = %f ",
        tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl),
        tabl_ind.koef.ravno(kol_tabl),index,pred_index,index*pred_index);

      if(index*pred_index >= 1.00999)
       {
        if(ff_prot != NULL)
          fprintf(ff_prot,"Больше или равно 1.01\n%f*%f*%f=",procind,index,pred_index);
        procind*=index*pred_index;

        if(ff_prot != NULL)
          fprintf(ff_prot,"%f",procind);

        procind=iceb_u_okrug(procind,0.001);

        if(ff_prot != NULL)
          fprintf(ff_prot," Округляем=%f",procind);
        
        pred_index=1.;
       }
      else
       {
        pred_index*=index;
//        pred_index=iceb_u_okrug(pred_index,0.001);
       }      
      if(ff_prot != NULL)
        fprintf(ff_prot,"\n");
    
/****************
      procind*=tabl_ind.koef.ravno(kol_tabl)/100.;
                    
      if(ff_prot != NULL)
        fprintf(ff_prot,"%02d.%04d %8.2f %f\n",
        tabl_ind.mi.ravno(kol_tabl),tabl_ind.gi.ravno(kol_tabl),
        tabl_ind.koef.ravno(kol_tabl),procind);
******************/
     }
   }
  procind=iceb_u_okrug(procind,0.001);
  if(ff_prot != NULL)
   fprintf(ff_prot,"Округляем %.4f\n",procind);
   
  if(ff_prot != NULL)
   {
  
   if(procind != 0.)
     fprintf(ff_prot,"Величина прироста индекса потребительских цен:%.4f*100-100=%.4f\n",
     procind,(procind*100.)-100.);
   else
     fprintf(ff_prot,"Величина прироста индекса потребительских цен:0.\n");
   }
  if(procind != 0.)
    procind=(procind*100.)-100.;


  /*Читаем все начисления за месяц индексации*/
  sprintf(strsql,"select knah,suma from Zarp where datz >= '%d-%d-01' \
and datz <= '%d-%d-31' and tabn=%d and prn='1' and suma <> 0.",
  gi,mi,gi,mi,tn);
  SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

  if(kolstr <= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не найдены начисления за %d.%d\n",mi,gi);
    continue;
   }

  suman=0.;
  while(cur.read_cursor(&row) != 0)
   {
    //Если за текущий месяц не должно входить в расчёт чтобы при пересчёте сумма не менялась
    if(atoi(row[0]) == kodindex && mmm == mi && ggg == gi)
      continue;
      
    if(provkodw(kodnvrindex,atoi(row[0])) >= 0)
      continue;
    
    suman+=atof(row[1]);
   }


  insum=suman;
  if(suman > sumaog)
    insum=sumaog;

  if(ff_prot != NULL)
      fprintf(ff_prot,"Максимальная сумма индексации:%.2f Процент индексации:%f\n\
Сумма начисленная:%.2f Сумма взятая для индексации:%.2f \n",sumaog,procind,suman,insum);
      
  sumain=insum*procind/100.;
  sumain=iceb_u_okrug(sumain,okrg);

  if(ff_prot != NULL)
     fprintf(ff_prot,"Код индексации:%d Сумма индексации:%.2f\n",kodindex,sumain);
  
  zp.mesn=mi; zp.godn=gi;

  bros[0]='\0';
  strcpy(zp.shet,shet);
  zapzarpw(&zp,sumain,zp.dz,mp,gp,0,shet,"",0,podr,"",wpredok);
  //zapzarp(zp.dz,mp,gp,tn,1,kodindex,sumain,shet,mi,gi,0,0,bros,podr,"",zp);

 }

//OSTANOV();
}
