/* $Id: pensw.c,v 1.16 2011-04-14 16:09:36 sasa Exp $ */
/*08.12.2010	02.08.2000	Белых А.И.	pensw.c
Расчет отчисления в пенсионный фонд
*/
#include <errno.h>
#include        <stdio.h>
#include        <math.h>
#include <unistd.h>
#include "buhg_g.h"

float pens_pp(int tabn,short mes,short god,FILE *ff,GtkWidget *wpredok);

extern short    kodpen;  /*Код пенсионных отчислений*/
extern float    procpen[2]; /*Процент отчисления в пенсионный фонд*/
extern SQL_baza bd;

double pensw(int tabn,short mes,short god,double suma,FILE *ff_prot,GtkWidget *wpredok)
{
double		bbpr,suma_pens_oth;

suma_pens_oth=0.;

class zar_read_tn1h nastr;
zar_read_tn1w(1,mes,god,&nastr,ff_prot,wpredok);

float proc_per=0.;
if((proc_per=pens_pp(tabn,mes,god,ff_prot,wpredok)) != 0.)
 {
  double suma_vr=suma;
  if(suma_vr > nastr.max_sum_for_soc)
   {
    suma_vr=nastr.max_sum_for_soc;
    if(ff_prot != NULL)
     fprintf(ff_prot,"Сумма с которой нужно начислять %.2f больше максимальной суммы с которой можно начислять налог %.2f.\n\
Берём в расчёт %.2f\n",suma,nastr.max_sum_for_soc,suma_vr);     
   }  

  suma_pens_oth=suma_vr*proc_per/100.;
    
  if(ff_prot != NULL)
   fprintf(ff_prot,"Персоналная сумма пенсионного отчисления %.2f*%.2f/100.=%.2f\n",suma_vr,proc_per,suma_pens_oth);
  return(suma_pens_oth);
 }

if(procpen[0] == procpen[1])
 {

  if(suma <= nastr.max_sum_for_soc)
   {
    suma_pens_oth=suma*procpen[0]/100.;
    if(ff_prot != NULL)
     fprintf(ff_prot,"%.2f*%.2f/100.=%.2f\n",suma,procpen[0],suma_pens_oth);
   }
  else
   {
    suma_pens_oth=nastr.max_sum_for_soc*procpen[0]/100.;
    if(ff_prot != NULL)
     fprintf(ff_prot,"Превышение %.2f\n%.2f*%.2f/100.=%.2f",
     nastr.max_sum_for_soc,nastr.max_sum_for_soc,procpen[0],suma_pens_oth);
  }

  return(suma_pens_oth);

 }

if(suma <= nastr.prog_min_tek)
 {

  bbpr=0.;
  if(procpen[0] != 0.)
    bbpr=procpen[0]/100.;
   //    printw("suma=%.2f snvr=%.2f nmpo=%.2f bbpr=%.2f\n",
  //    suma,snvr,nmpo,bbpr);
  suma_pens_oth=suma*bbpr;
  
  if(ff_prot != NULL)
   fprintf(ff_prot,"%s %.2f-%.2f%% %s %.2f %s %.2f\n",
  gettext("Пенсионные"),suma_pens_oth,procpen[0],
  gettext("Сумма"),suma,
  gettext("Ограничение"),nastr.prog_min_tek);
 }
else
 {
    double suma1=nastr.prog_min_tek*procpen[0]/100.;

    double suma2=0.;
    
    if(suma <= nastr.max_sum_for_soc)
      suma2=(suma-nastr.prog_min_tek)*procpen[1]/100.;
    else
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Первышение %.2f\n",nastr.max_sum_for_soc);
      suma2=(nastr.max_sum_for_soc-nastr.prog_min_tek)*procpen[1]/100.;
     }
    suma_pens_oth=suma1+suma2;
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"%.2f*%.2f/100=%.2f\n",nastr.prog_min_tek,procpen[0],suma1);
      if(suma <= nastr.max_sum_for_soc)
        fprintf(ff_prot,"(%.2f-%.2f)*%.2f/100=%.2f\n",suma,nastr.prog_min_tek,procpen[1],suma2);
      else      
        fprintf(ff_prot,"(%.2f-%.2f)*%.2f/100=%.2f\n",nastr.max_sum_for_soc,nastr.prog_min_tek,procpen[1],suma2);
      fprintf(ff_prot,"%.2f+%.2f=%.2f\n",suma1,suma2,suma_pens_oth);
     }    
 }
return(suma_pens_oth);
}
/**********************/
/*смотрим файл с персоналными процентами отчисления в пенсионный фонд*/
/***********************************************************************/
float pens_pp(int tabn,short mes,short god,FILE *ff_prot,GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
if(ff_prot != NULL)
 fprintf(ff_prot,"Проверяем список персональных процентов отчисления в пенсионный фонд.\n");

sprintf(strsql,"select str from Alx where fil='zarsppo.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }

if(kolstr == 0)
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не найдены настройки zarsppo.alx\n");
  return(0.);
 }

 
float proc=0.;
int tabnom=0;
short d,m,g;
char data[100];
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue; 

  if(iceb_u_polen(row_alx[0],&tabnom,1,'|') != 0)
   continue;  
  if(tabnom == 0)
   continue;
  if(tabnom != tabn)
   continue;

  if(iceb_u_polen(row_alx[0],data,sizeof(data),3,'|') != 0)
   continue;  

  if(data[0] != '\0')
  if(iceb_u_rsdat(&d,&m,&g,data,1) != 0)
   {
    if(ff_prot != NULL)
     {
      fprintf(ff_prot,"%s",row_alx[0]);
      fprintf(ff_prot,"Не правильно введена дата!\n");
     }
    continue;
   }     

  if(iceb_u_sravmydat(1,mes,god,1,m,g) >= 0 || data[0] == '\0')
   {
    if(ff_prot != NULL)
      fprintf(ff_prot,"%s",row_alx[0]);
    iceb_u_polen(row_alx[0],&proc,2,'|');
    /*смотрим дальше так как могут быть настройки по более поздним датам*/
   }
 }

if(ff_prot != NULL)
  fprintf(ff_prot,"Персональный процент=%.2f\n",proc);
return(proc);
}


