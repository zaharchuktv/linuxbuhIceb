/* $Id: findnahmw.c,v 1.7 2011-02-21 07:35:51 sasa Exp $ */
/*12.02.2010    10.12.1993      Белых А.И.      findnahmw.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/

#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "zarp1.h"


int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,double *suma_nfnz,double *suma_nfnz_b,double *suma_zarp_o,double *sumabu,
double *sumanb,double *suma_zarp_b,double *nah,double *suma_boln_rm,FILE *ff);

extern short    *knvr;/*Коды начислений не входящие в расчет подоходного налога*/
extern short    *kuvr;/*Коды удержаний входящие в расчет подоходного нолога*/
extern char	*shetb; /*Бюджетные счета*/
extern class ZAR zar;
extern  short   *kodbl; /*Код больничного*/
extern short    *kuvvr_pens_sr; //Коды удержаний входящие в расчет при отчислении с работника
extern short    *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern SQL_baza bd;

double          findnahmw(short mr, //Месяц расчета
short mt, //1- подоходный налог
          //2 - пенсионные отчисления
          //3 - профсоюз
          //4 - отчисления в фонд безработицы
          //5 - соц-страх
short *kodi, //Дополнительные коды не входящие в расчет*/
FILE *ff,
double *sumanb, //Сумма не бюджетная
double *sumabu, //Сумма бюджетная
double *suma_nfnz, //Общая сумма начисленная не денежными формами оплаты
double *suma_nfnz_b, //Бюджетная сумма начисленная не денежными формами оплаты
double *suma_zarp_o, //Сумма зарплатных начислений (на которые распространяется социальная льгота) общая
double *suma_zarp_b, //Сумма зарплатных начислений (на которые распространяется социальная льгота) бюджета
double *suma_boln_rm) /*Сумма больничных за расчётный месяц*/
{
double          nah;
int             i;

//printw("\nfindnahm-%d\n",mt);
*suma_zarp_o=*suma_zarp_b=0.;
*sumanb=*sumabu=nah=0.;
*suma_nfnz=*suma_nfnz_b=0.;
*suma_boln_rm=0.;

for(i=0;i<razm;i++)
 {
  if( findnagm_prov(mt,mr,zar.prnu[i],zar.mes[i],zar.knu[i],zar.sm[i],zar.sheta[i],
  kodi,suma_nfnz,suma_nfnz_b,suma_zarp_o,sumabu,sumanb,suma_zarp_b,&nah,suma_boln_rm,ff) != 0)
    break;

 }

//fprintf(ff,"findnahv-suma_nfnz=%.2f\n",*suma_nfnz);

return(nah);
}

/**************/
/*Чтение в базе*/
/***************/

double          findnahmw(short mr,short gr, //Дата расчёта
int tabn,
short mt, //1- подоходный налог
          //2 - пенсионные отчисления
          //3 - профсоюз
          //4 - отчисления в фонд безработицы
          //5 - соц-страх
short *kodi, //Дополнительные коды не входящие в расчет*/
FILE *ff,
double *sumanb, //Сумма не бюджетная
double *sumabu, //Сумма бюджетная
double *suma_nfnz, //Общая сумма начисленная не денежными формами оплаты
double *suma_nfnz_b, //Бюджетная сумма начисленная не денежными формами оплаты
double *suma_zarp_o, //Сумма зарплатных начислений (на которые распространяется социальная льгота) общая
double *suma_zarp_b, //Сумма зарплатных начислений (на которые распространяется социальная льгота) бюджета
double *suma_boln_rm, /*Сумма больничных за расчётный месяц*/
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
double nah;

*suma_zarp_o=*suma_zarp_b=0.;
*sumanb=*sumabu=nah=0.;
*suma_nfnz=*suma_nfnz_b=0.;
*suma_boln_rm=0.;

sprintf(strsql,"select prn,knah,suma,shet,mesn from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d",gr,mr,1,gr,mr,31,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  findnagm_prov(mt,mr,atoi(row[0]),atoi(row[4]),atoi(row[1]),atof(row[2]),row[3],
  kodi,suma_nfnz,suma_nfnz_b,suma_zarp_o,
  sumabu,sumanb,suma_zarp_b,&nah,suma_boln_rm,ff);

 }

return(nah);

}
/*******************************************/
/*Проверка общая для двух подпрограмм*/
/*****************************************/
//Если вернули 0-всё впорядке 1-прервать цикл
int findnagm_prov(short mt,short mr,int prnu,short mes,int knu,double suma,char *sheta,
short *kodi,
double *suma_nfnz,
double *suma_nfnz_b,
double *suma_zarp_o,
double *sumabu,
double *sumanb,
double *suma_zarp_b,
double *nah,
double *suma_boln_rm, /*Сумма больничных за расчётный месяц*/
FILE *ff_prot)
{

if(prnu == 0)
 return(1);
 
//  printw("%.2f\n",suma);

if(prnu == 1)
 {
//     if(ff_prot != NULL)
//        fprintf(ff_prot,"mr=%d mes[%d]=%d mt=%d knu=%d\n",mr,i,mes[i],mt,knu);

  /*Узнаём сумму больничных за расчётный месяц*/
   if(mes == mr)
     if(provkodw(kodbl,knu) >= 0)
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Больничный %d %.2f\n",knu,suma);
       *suma_boln_rm+=suma;
      }

  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчета профсоюзного взносов включается*/
  /*Для этих удержаний делается перерасчет по этим больничным отдельно*/
  
  if(mt == 1 || mt == 2 || mt == 5)
   if(mes != mr)
     if( provkodw(kodbl,knu) >= 0)
      {
       if(ff_prot != NULL)
         fprintf(ff_prot,"Не вошло в расчёт %d %.2f\n",knu,suma);
       return(0);
      }
  /*Коды начислений не входящие в расчет подоходного налога*/
  if(mt == 1)
  if(provkodw(knvr,knu) >= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не вошло в расчёт %d %.2f\n",knu,suma);
    return(0);
   }
   
  if(provkodw(kodi,knu) >= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не вошло в расчёт %d %.2f\n",knu,suma);
    return(0);
   }
  *suma_nfnz+=knnfndsw(1,knu,sheta,suma,suma_nfnz_b,ff_prot);

/*********
  if(provkodw(knnf,knu) >= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не денежная форма оплаты (общая) %d %.2f\n",knu,suma);
    *suma_nfnz+=suma;
    if(iceb_u_proverka(shetb,sheta,0,1) == 0)
     {
      if(ff_prot != NULL)
       fprintf(ff_prot,"Не денежная форма оплаты (бюджет) %d %.2f\n",knu,suma);
      *suma_nfnz_b+=suma;  
     }
   }
************/  
  if(provkodw(kn_bsl,knu) < 0)
   {
    *suma_zarp_o+=suma;
   }

  if(shetb != NULL ) 
   {
    /*Узнаем какая часть суммы бюджетных счетов и всех остальных*/
    if(iceb_u_proverka(shetb,sheta,0,0) == 0)
     {
      *sumabu+=suma;
      if(provkodw(kn_bsl,knu) < 0)
        *suma_zarp_b+=suma;
     }
    else
        *sumanb+=suma;
    
/*************    
    int i1=0;
    if(pole1(shetb,sheta,',',0,&i1) == 0)
        *sumabu+=suma;
    else
        *sumanb+=suma;
**************/


   }

  *nah+=suma;
 
 }

if(prnu == 2)
 {
  /*Коды удержаний входящие в расчет подоходного налога*/

  if(mt == 1)  /*Для пенсии и профсоюза входят*/
  if(provkodw(kuvr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    if(ff_prot != NULL)
        fprintf(ff_prot,"Вошло в расчет удержание %d %.2f\n",knu,suma);
    return(0);
   }

  if(mt == 2)  //Коды удержаний входящие в расчет для пенсионного отчисления с работника
  if(provkodw(kuvvr_pens_sr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    if(ff_prot != NULL)
        fprintf(ff_prot,"Вошло в расчет удержание %d %.2f\n",knu,suma);
    return(0);
   }

 }
return(0);
}

