/*$Id: podohrs1w.c,v 1.11 2011-04-14 16:09:36 sasa Exp $*/
/*16.02.2010	15.12.2003	Белых А.И.	podohrs1w.c
Расчет подоходного налога в соответствии с новым законом вступающим в силу с 1.1.2004г.
Возвращяем величину подоходного налога
*/
#include        <errno.h>
#include        "buhg_g.h"

extern double   okrg;
//extern float  prog_min_ng; //Прожиточный минимум на начало года
extern float  kof_prog_min; //Коэффициент прожиточного минимума
//extern double minzar_ng; /*Минимальная зарплата*/		
extern float  ppn; //Процент подоходного налога
extern float  pomzp; //Процент от минимальной зароботной платы
extern SQL_baza bd;

double  podohrs1w(int tabn,
short mr,short gr,
double suma_zar, //Сумма зарплаты на которую распространяется соц-льгота
double suma_soc_ot, //сумма соц-отчислений c зарплаты
double suma_bsl, //Сумма без социальной льготы - не зарплата
double suma_ndfv, //Сумма неденежных форм выплаты
FILE *ff_prot,
GtkWidget *wpredok)
{
char stroka[1024];
float procent=0.;
double nalog=0.;
double lgota=0.;
double  maxsum=0.;
double suma_bso=0.;
short du=0,mu=0,gu=0;

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,ff_prot,wpredok);


//читаем дату увольнения
sprintf(stroka,"select datk from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);

SQLCURSOR cur;
SQL_str   row;

if(iceb_sql_readkey(stroka,&row,&cur,wpredok) == 1)
 iceb_u_rsdat(&du,&mu,&gu,row[0],2);
  
maxsum=nastr.prog_min_ng*kof_prog_min;
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nМинимальная зарплата на начало года=%.2f\n",nastr.minzar_ng);
  fprintf(ff_prot,"Прожиточный минимум на начало года=%.2f\n",nastr.prog_min_ng);
  fprintf(ff_prot,"Коэффициент прожиточного минимума=%.2f\n",kof_prog_min);
  fprintf(ff_prot,"Процент подоходного налога=%.2f\n",ppn);
  fprintf(ff_prot,"Процент от минимальной зарплаты=%.2f\n",pomzp);
  fprintf(ff_prot,"Сумма на которую не распростаняется социальная льгота=%.2f\n",suma_bsl);
  fprintf(ff_prot,"Сумма на которую распростаняется социальная льгота=%.2f\n",suma_zar);
  fprintf(ff_prot,"Сумма социальных отчислений=%.2f\n",suma_soc_ot);
  fprintf(ff_prot,"Сумма неденежных форм выплаты=%.2f\n",suma_ndfv);
  fprintf(ff_prot,"Дата расчёта зарплаты %d.%d\n",mr,gr);
  fprintf(ff_prot,"Дата увольнения %d.%d.%d\n",du,mu,gu);
  fprintf(ff_prot,"Максимальный доход для льгот:%.2f*%.2f = %.2f\n",nastr.prog_min_ng,kof_prog_min,maxsum);
 }

maxsum=iceb_u_okrug(maxsum,10.);

if(ff_prot != NULL)
  fprintf(ff_prot,"Округлили %.2f\n",maxsum);

//double suma_ud=0.;//сумма увеличения дохода

if(suma_ndfv != 0.)
  rudnfvw(&suma_zar,suma_ndfv,ff_prot);


if(ff_prot != NULL)
  fprintf(ff_prot,"Сумма без соц-отчислений=%.2f %.2f=%.2f\n",suma_zar,suma_soc_ot,suma_zar+suma_soc_ot);

if(suma_zar > 0.)  //Отрицательные поэтому прибавляем
  suma_bso=suma_zar+suma_soc_ot;

int kollgot=0;
class iceb_u_str kod_lgot("");
procent=zarlgotw(tabn,mr,gr,&kollgot,&kod_lgot,ff_prot,wpredok);


if(ff_prot != NULL)
 {   
  fprintf(ff_prot,"Количество социальных льгот=%d\n",kollgot);
  fprintf(ff_prot,"Максимальная сумма для которой применяется льгота %.2f*%d=%.2f\n",
  maxsum,kollgot,maxsum*kollgot);
 }

maxsum*=kollgot;

if(maxsum < suma_zar || (mu == mr && gu == gr))
 {

  nalog=(suma_bso+suma_bsl)*ppn/100.;

  if(ff_prot != NULL)
   {
    if(mu == mr && gu == gr)
      fprintf(ff_prot,"Месяц увольнения ! Льготы по подоходному налогу не применяються.\n");
    else
      fprintf(ff_prot,"*Льготы по подоходному налогу не применяються %.2f < %.2f\n",maxsum,suma_zar);

   fprintf(ff_prot,"(%.2f+%.2f)*%.2f/100=%.2f\n",suma_bso,suma_bsl,ppn,nalog);
  }

  nalog=iceb_u_okrug(nalog,okrg)*(-1);
  return(nalog);
 }




if(procent == 0.)
 {
  lgota=0.;
  if(ff_prot != NULL)
    fprintf(ff_prot,"Не имеет льгот!\n");

  nalog=(suma_bso+suma_bsl)*(ppn/100);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Подоходный налог  - ((%.2f+%.2f)*%.2f/100=%.2f\n",suma_bso,suma_bsl,ppn,nalog);

  nalog=iceb_u_okrug(nalog,okrg)*(-1);

  return(nalog);
 }
else
 {

      
  lgota=nastr.minzar_ng*(procent/100)*(pomzp/100);
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"Расчетная сумма льготы=%.2f*%.2f/100*%.2f/100=%.2f\n",nastr.minzar_ng,procent,pomzp,lgota);
 }




if(suma_zar+suma_bsl <= maxsum)
 {
  if(suma_bso < lgota) /*Если отнять будет отрицательное число*/
   {
    if(ff_prot != NULL)
        fprintf(ff_prot,"Cумма начисления %.2f меньше льготы %.2f\n",suma_bso,lgota);

    if(suma_bsl != 0.)
     {
      nalog=suma_bsl*(ppn/100);
      if(ff_prot != NULL)
       {
        fprintf(ff_prot,"Подоходный налог  - %.2f*%.2f/100=%.2f\n",suma_bsl,ppn,nalog);
       }
     }
    else
     {
      if(ff_prot != NULL)
        fprintf(ff_prot,"Налог не берётся!\n");
      return(0.);
     }

   }
  else
   {
    nalog=((suma_bso-lgota)+suma_bsl)*(ppn/100);
    if(ff_prot != NULL)
      fprintf(ff_prot,"Подоходный налог  - ((%.2f-%.2f)+%.2f)*%.2f/100=%.2f\n",suma_bso,lgota,suma_bsl,ppn,nalog);
   }

  nalog=iceb_u_okrug(nalog,okrg)*(-1);
  
 }
else
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Льгота не применяется так как %.2f+%.2f > %.2f\n",suma_zar,suma_bsl,maxsum);
  nalog=(suma_bso+suma_bsl)*(ppn/100);
  nalog=iceb_u_okrug(nalog,okrg)*(-1);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Подоходный налог  - (%.2f+%.2f)*%.2f/100=%.2f\n",suma_bso,suma_bsl,ppn,nalog);
 }

return(nalog);

}
