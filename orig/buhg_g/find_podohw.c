/* $Id: find_podohw.c,v 1.7 2011-02-21 07:35:51 sasa Exp $ */
/*04.09.2006    04.02.2004      Белых А.И.      find_podohw.c
Поиск всех начислений на нужного человека за указанный
месяц и год
*/
#include <stdlib.h>
#include        <math.h>
#include        "buhg_g.h"
#include        "zarp1.h"

int find_podoh_prov(short mr,int prnu,short mes,int knu,double suma,
double *suma_ndfv,double *sumabu,double *sumanb,double *nah,char *sheta,FILE *ff);

extern short    *knvr;/*Коды начислений не входящие в расчет налогов*/
extern short    *kuvr;/*Коды удержаний входящие в расчет налогов*/
extern char	*shetb; /*Бюджетные счета*/
extern class ZAR zar;
extern  short   *kodbl; /*Код больничного*/
extern SQL_baza bd;

double find_podohw(short mr, //Месяц расчета
FILE *ff,
double *suma_ndfv, //Сумма не денежных форм выплат
double *sumanb, //Сумма не бюджетная
double *sumabu) //Сумма бюджетная
{
double          nah;
int             i;

//printw("\nfindnahm-%d\n",mt);

*suma_ndfv=*sumanb=*sumabu=nah=0.;

for(i=0;i<razm;i++)
 {

  if(find_podoh_prov(mr,zar.prnu[i],zar.mes[i],zar.knu[i],zar.sm[i],
  suma_ndfv,sumabu,sumanb,&nah,zar.sheta[i],ff) != 0)
    break;
 }

//printw("nah=%.2f\n",nah);
//OSTANOV();
return(nah);
}
/****************/
/*Чтение в базе*/
/****************/

double          find_podohw(short mr,short gr, //Дата расчета
int tabn,
FILE *ff,
double *suma_ndfv, //Сумма не денежных форм выплат
double *sumanb, //Сумма не бюджетная
double *sumabu, //Сумма бюджетная
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
int kolstr;
double nah;

*suma_ndfv=*sumanb=*sumabu=nah=0.;

sprintf(strsql,"select prn,knah,suma,shet,mesn from Zarp where datz >= '%04d-%02d-%02d' \
and datz <= '%04d-%02d-%02d' and tabn=%d",gr,mr,1,gr,mr,31,tabn);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {

  if(find_podoh_prov(mr,atoi(row[0]),atoi(row[4]),atoi(row[1]),atof(row[2]),
  suma_ndfv,sumabu,sumanb,&nah,row[3],ff) != 0)
    break;

 }

return(nah);


}



/********************************/
/*Проверка */
/**********************************/

int find_podoh_prov(short mr,int prnu,short mes,int knu,double suma,
double *suma_ndfv,double *sumabu,double *sumanb,double *nah,char *sheta,FILE *ff_prot)
{
if(prnu == 0)
 return(1);

//  printw("%.2f\n",suma);

if(prnu == 1)
 {
//     if(ff_prot != NULL)
//        fprintf(ff_prot,"mr=%d zar.mes[%d]=%d mt=%d zar.knu=%d\n",mr,i,mes,mt,knu);


  /*Если больничный не за текущий месяц то в общюю сумму не включ.*/
  /*Для расчета профсоюзного взносов включается*/
  /*Для этих удержаний делается перерасчет по этим больничным отдельно*/
  
  if(mes != mr)
    if( provkodw(kodbl,knu) >= 0)
     {
      if(ff_prot != NULL)
        fprintf(ff_prot,"Не вошло в расчёт %d %.2f\n",knu,suma);
      return(0);
     }

  /*Коды начислений не входящие в расчет подоходного налога*/
  if(provkodw(knvr,knu) >= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не вошло в расчёт %d %.2f\n",knu,suma);
    return(0);
   }
  double budget=0.;
  *suma_ndfv+=knnfndsw(0,knu,"",suma,&budget,ff_prot);

/**********
  if(provkodw(knnf,knu) >= 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не денежная форма выплаты %d %.2f\n",knu,suma);
    *suma_ndfv+=suma;
   }
*************/   

  if(shetb != NULL ) 
   {
    /*Узнаем какая часть суммы бюджетных счетов и всех остальных*/
    if(iceb_u_proverka(shetb,sheta,0,1) == 0)
        *sumabu+=suma;
    else
        *sumanb+=suma;
   }

  *nah+=suma;
 
 }

if(prnu == 2)
 {
  /*Коды удержаний входящие в расчет подоходного налога*/

  if(provkodw(kuvr,knu) >= 0)
   {
    *nah+=suma; //Удержание отрицательное поэтому для того, чтобы уменьшить сумму мы его просто прибавляем
    if(ff_prot != NULL)
        fprintf(ff_prot,"Вошло в расчет удержание %d %.2f\n",knu,suma);
    return(0);
   }

 }
return(0);

}
