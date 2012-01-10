/*$Id: zarsbslw.c,v 1.6 2011-08-29 07:13:44 sasa Exp $*/
/*09.08.2011	13.08.2009	Белых А.И.	zarsbslw.c
Определение суммы начислений на которые не распространяется социальная льгота
Возвдращает сумму с начислением
*/
#include <stdlib.h>
#include "buhg_g.h"
#include "zarp1.h"

extern struct ZAR zar;
extern short *knnf; //Коды начислений недежными формами 
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern  short   *kodbl; /*Код больничного*/
extern char	*shetb; /*Бюджетные счета начислений*/
extern float pnds; 
extern double okrg;




double zarsbsl2(short mp,double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b,  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
FILE *ff_prot);

double zarsbsl1(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b,  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
FILE *ff_prot);




extern SQL_baza bd;




double zarsbslw(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b,  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
int metka_ras, //0-смотреть начисления в массиве 1-в базе
FILE *ff_prot)
{

if(metka_ras == 0)
  return(zarsbsl2(mp,suma_bsl_b,suma_bsl_start,suma_bsl_start_b,ff_prot));

if(metka_ras == 1)
 return(zarsbsl1(tabnom,mp,gp,suma_bsl_b,suma_bsl_start,suma_bsl_start_b,ff_prot));

return(0.);

}
/*******************/
/*поиск в базе*/
/*******************/
double zarsbsl1(int tabnom,short mp,short gp,
double *suma_bsl_b,/*сумма с увеличением для бюджета*/
double *suma_bsl_start, /*Сумма без социальной льготы без начисления в чистом виде общая*/
double *suma_bsl_start_b,  /*Сумма без социальной льготы без начисдения в чистом виде бюджет*/
FILE *ff_prot)
{
if(ff_prot != NULL)
 fprintf(ff_prot,"%s-Вычисляем сумму начислений на которые не распространяется социальная льгота-смотрим в базе\n",__FUNCTION__);
char strsql[512];
double		sum;
*suma_bsl_b=sum=0.;
int kolstr=0;
SQL_str row;

if(kn_bsl != NULL)
  return(0.);

sprintf(strsql,"select knah,suma,mesn,shet from Zarp where \
datz >= '%04d-%02d-01' and datz <= '%04d-%02d-31' and tabn=%d and prn='1' and \
suma <> 0. order by prn,knah asc",gp,mp,gp,mp,tabnom);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
  return(1);
short mes=0;
int kodnah=0;
double suma_nah=0.;

while(cur.read_cursor(&row) != 0)
 {
  kodnah=atoi(row[0]);
  if(provkodw(kn_bsl,kodnah) >= 0)
   {
     if(ff_prot != NULL)
      {
       fprintf(ff_prot,"%s %s\n",row[0],row[1]);
       fprintf(ff_prot,"Подошло\n");
      }
     
     mes=atoi(row[2]);
     suma_nah=atof(row[1]);
     //Если больничный за предыдущие периоды, то игнорируем так как 
     //он уже учтен в findnahm
     if(mes != mp && provkodw(kodbl,kodnah) >= 0)
      {
       if(ff_prot != NULL)
        fprintf(ff_prot,"Больничный за предыдущий период\n");
      }
     else
      {
       if(provkodw(knnf,kodnah) >= 0)
        {
         double suma=suma_nah;
         *suma_bsl_start+=suma_nah;   
         if(ff_prot != NULL)
           fprintf(ff_prot,"Код начисления %d сумма=%.2f\n",kodnah,suma);
         if(provkodw(knnf_nds,kodnah) >= 0)
          {
           double suma_nds=suma*pnds/100.;
           suma_nds=iceb_u_okrug(suma_nds,okrg);
           suma+=suma_nds;
           if(ff_prot != NULL)
              fprintf(ff_prot,"Начисляем НДС %.2f+%.2f=%.2f\n",suma_nah,suma_nds,suma);
          }                  
         rudnfvw(&suma,suma,ff_prot); 

         sum+=suma;
         if(iceb_u_proverka(shetb,row[3],0,1) == 0)
          {
           *suma_bsl_start_b+=suma_nah;   
           *suma_bsl_b+=suma;
          }
        }        
       else
        {
         sum+=suma_nah;
         *suma_bsl_start+=suma_nah;   
         if(iceb_u_proverka(shetb,row[3],0,1) == 0)
          {
           *suma_bsl_b+=suma_nah;
           *suma_bsl_start_b+=suma_nah;   
          }
        }

      }
   }
 }
return(sum);
}
/*************************************/
/*просмотр массива*/
/***************************************/


double zarsbsl2(short mp,double *suma_bsl_b,
double *suma_bsl_bnds, /*Сумма без социальной льготы без НДС в чистом виде без начисления*/
double *suma_bsl_bnds_b,  /*Сумма без социальной льготы без НДС бюджет*/
FILE *ff_prot)
{
if(ff_prot != NULL)
 fprintf(ff_prot,"Вычисляем сумму начислений на которые не распространяется социальная льгота\n");
short		i,i1;
double		sum;
int tz=1;
*suma_bsl_b=sum=0.;
if(kn_bsl != NULL)
for(i=1; i <= kn_bsl[0] ;i++)
  for(i1=0; i1 < razm; i1++)
   {
    if(tz != zar.prnu[i1])
     continue;
    if(zar.prnu[i1] == 0)
     continue;
   if(tz == zar.prnu[i1] && kn_bsl[i] == zar.knu[i1])
    {
     //Если больничный за предыдущие периоды, то игнорируем так как 
     //он уже учтен в findnahm
     if(zar.mes[i1] != mp && provkodw(kodbl,zar.knu[i1]) >= 0)
      {

     //           printw("prosmas-учтено раньше %d %.2f\n",zar.knu[i],zar.sm[i]);
       //         OSTANOV();         
      }
     else
      {
      
       if(provkodw(knnf,zar.knu[i1]) >= 0)
        {
         double suma=zar.sm[i1];
         *suma_bsl_bnds+=zar.sm[i1];   
         if(ff_prot != NULL)
           fprintf(ff_prot,"Код начисления %d сумма=%.2f\n",zar.knu[i1],suma);
         if(provkodw(knnf_nds,zar.knu[i1]) >= 0)
          {
           double suma_nds=suma*pnds/100.;
           suma_nds=iceb_u_okrug(suma_nds,okrg);
           suma+=suma_nds;
           if(ff_prot != NULL)
              fprintf(ff_prot,"Начисляем НДС %.2f+%.2f=%.2f\n",zar.sm[i1],suma_nds,suma);
          }                  
         rudnfvw(&suma,suma,ff_prot); 

         sum+=suma;
         if(iceb_u_proverka(shetb,zar.sheta[i1],0,1) == 0)
          {
           *suma_bsl_bnds_b+=zar.sm[i1];   
           *suma_bsl_b+=suma;
          }
        }        
       else
        {
         sum+=zar.sm[i1];
         if(iceb_u_proverka(shetb,zar.sheta[i1],0,1) == 0)
          {
           *suma_bsl_b+=zar.sm[i1];
          }
        }

      }
    }  
 }

return(sum);
}

