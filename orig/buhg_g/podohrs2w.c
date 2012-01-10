/*$Id: podohrs2w.c,v 1.4 2011-04-14 16:09:36 sasa Exp $*/
/*07.04.2011	15.12.2003	Белых А.И.	podohrs2w.c
Расчет подоходного налога в соответствии с новым законом вступающим в силу с 1.1.2004г.
Возвращяем величину подоходного налога
После 1.1.2011
*/
#include        <errno.h>
#include        "buhg_g.h"

double podohrs2_pr(float proc1,float proc2,double suma,FILE *ff_prot);
double podohrs2_pr1(float proc1,float proc2,double suma_zar,double suma_esv,double suma_bsl,double mizar_gn,float max_sum_for_soc,FILE *ff_prot);

extern double   okrg;
//extern float  prog_min_ng; //Прожиточный минимум на начало года
extern float  kof_prog_min; //Коэффициент прожиточного минимума
//extern double minzar_ng; /*Минимальная зарплата*/		
extern float  ppn; //Процент подоходного налога
extern float  pomzp; //Процент от минимальной заработной платы

extern SQL_baza bd;
extern double proc_esv_prim; /*применённый процент единого социального взноса*/
//extern float max_sum_for_soc; //Максимальная сумма с которой начисляются соц.отчисления

double podohrs2w(int tabn,
short mr,short gr,
double suma_zar, //Сумма зарплаты на которую распространяется соц-льгота
double suma_soc_ot, //сумма соц-отчислений c зарплаты
double suma_bsl, //Сумма без социальной льготы - не зарплата
double suma_ndfv, //Сумма неденежных форм выплаты
int metka_zap, /*0-делать запись 1-нет*/
int metka_zap_lgot, /*0-записывать/удалыть льготу в карточку 1-нет*/
FILE *ff_prot,
GtkWidget *wpredok)
{
char stroka[1024];
float procent=0.;
float pr_b10mz=17.; /* Процент который берётся если сумма больше 10 минимальных зарплат*/
double nalog=0.;
double lgota=0.;
double  maxsum=0.;
double suma_bso=0.;
short du=0,mu=0,gu=0;
class zar_read_tn1h nast_mz;
SQLCURSOR cur;
SQL_str   row;

//читаем дату увольнения
sprintf(stroka,"select datk from Zarn where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
if(sql_readkey(&bd,stroka,&row,&cur) == 1)
 iceb_u_rsdat(&du,&mu,&gu,row[0],2);

zar_read_tn1w(1,mr,gr,&nast_mz,ff_prot,wpredok);
  
maxsum=nast_mz.prog_min_ng*kof_prog_min;
if(ff_prot != NULL)
 {
  fprintf(ff_prot,"\nМинимальная зарплата на начало года=%.2f\n",nast_mz.minzar_ng);
  fprintf(ff_prot,"Прожиточный минимум на начало года=%.2f\n",nast_mz.prog_min_ng);
  fprintf(ff_prot,"Коэффициент прожиточного минимума=%.2f\n",kof_prog_min);
  fprintf(ff_prot,"Процент подоходного налога=%.2f\n",ppn);
  fprintf(ff_prot,"Процент от минимальной зарплаты=%.2f\n",pomzp);
  fprintf(ff_prot,"Сумма на которую не распростаняется социальная льгота=%.2f\n",suma_bsl);
  fprintf(ff_prot,"Сумма на которую распростаняется социальная льгота=%.2f\n",suma_zar);
  fprintf(ff_prot,"Сумма социальных отчислений=%.2f\n",suma_soc_ot);
  fprintf(ff_prot,"Сумма неденежных форм выплаты=%.2f\n",suma_ndfv);
  fprintf(ff_prot,"Дата расчёта зарплаты %d.%d\n",mr,gr);
  fprintf(ff_prot,"Дата увольнения %d.%d.%d\n",du,mu,gu);
  fprintf(ff_prot,"Максимальный доход для льгот:%.2f*%.2f = %.2f\n",nast_mz.prog_min_ng,kof_prog_min,maxsum);
 }

maxsum=iceb_u_okrug(maxsum,10.);

if(ff_prot != NULL)
  fprintf(ff_prot,"Округлили %.2f\n",maxsum);

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

if(maxsum < suma_zar)
 {
  /*Льгота не применялась*/
  if(metka_zap == 0 && metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='' where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }
  nalog=podohrs2_pr1(ppn,pr_b10mz,suma_zar,suma_soc_ot,suma_bsl,nast_mz.minzar_ng,nast_mz.max_sum_for_soc,ff_prot);

  if(ff_prot != NULL)
   {
    fprintf(ff_prot,"*Льготы по подоходному налогу не применяються %.2f < %.2f\n%s\n",maxsum,suma_zar,stroka);

   }

  nalog=iceb_u_okrug(nalog,okrg);
  return(nalog);
 }




if(procent == 0.)
 {
  lgota=0.;
  /*Льгота не применялась*/
  if(metka_zap == 0 && metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='' where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }
  if(ff_prot != NULL)
    fprintf(ff_prot,"Не имеет льгот!\n%s\n",stroka);

  
  nalog=podohrs2_pr1(ppn,pr_b10mz,suma_zar,suma_soc_ot,suma_bsl,nast_mz.minzar_ng,nast_mz.max_sum_for_soc,ff_prot);

  if(ff_prot != NULL)
    fprintf(ff_prot,"Подоходный налог  - ((%.2f+%.2f)*%.2f/100=%.2f\n",suma_bso,suma_bsl,ppn,nalog);

  nalog=iceb_u_okrug(nalog,okrg);

  return(nalog);
 }
else
 {

  lgota=nast_mz.minzar_ng*(procent/100)*(pomzp/100);
  
  if(ff_prot != NULL)
    fprintf(ff_prot,"Расчетная сумма льготы=%.2f*%.2f/100*%.2f/100=%.2f\n",nast_mz.minzar_ng,procent,pomzp,lgota);
 }


if(suma_zar+suma_bsl <= maxsum)
 {
  /*Льгота применялась*/
  if(kod_lgot.getdlinna() <= 1)
   kod_lgot.new_plus("01");
  if(metka_zap == 0 && metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='%s' where tabn=%d and god=%d and mes=%d",kod_lgot.ravno(),tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"Льгота применяется так как %.2f+%.2f <= %.2f\n%s\n",suma_zar,suma_bsl,maxsum,stroka);


  if(suma_bso < lgota) /*Если отнять будет отрицательное число*/
   {
    if(ff_prot != NULL)
        fprintf(ff_prot,"Cумма начисления %.2f меньше льготы %.2f\n",suma_bso,lgota);

    if(suma_bsl != 0.)
     {
      nalog=podohrs2_pr1(ppn,pr_b10mz,0.,0.,suma_bsl,nast_mz.minzar_ng,nast_mz.max_sum_for_soc,ff_prot);
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
    nalog=podohrs2_pr1(ppn,pr_b10mz,suma_zar-lgota,suma_soc_ot,suma_bsl,nast_mz.minzar_ng,nast_mz.max_sum_for_soc,ff_prot);
   }

  nalog=iceb_u_okrug(nalog,okrg);

  
 }
else
 {
  /*Льгота не применялась*/
  if(metka_zap == 0 && metka_zap_lgot == 0)
   {
    sprintf(stroka,"update Zarn set lgot='' where tabn=%d and god=%d and mes=%d",tabn,gr,mr);
    iceb_sql_zapis(stroka,1,0,wpredok);
   }
  if(ff_prot != NULL)
   fprintf(ff_prot,"Льгота не применяется так как %.2f+%.2f > %.2f\n%s\n",suma_zar,suma_bsl,maxsum,stroka);


    nalog=podohrs2_pr1(ppn,pr_b10mz,suma_zar,suma_soc_ot,suma_bsl,nast_mz.minzar_ng,nast_mz.max_sum_for_soc,ff_prot);

 }
return(nalog);

}
/***********************/
/*Взятие процента*/
/************************/
/**************************************
double podohrs2_pr(float proc1,float proc2,double suma,FILE *ff_prot)
{
double predel=minzar_ng*10.;
double suma_nal=0.;
if(ff_prot != NULL)
 fprintf(ff_prot,"Рассчитываем налог\n");
if(suma <= minzar_ng*10.)
 {
  suma_nal=suma*proc1/100.;
  if(ff_prot != NULL)
   fprintf(ff_prot,"Cумма %.2f меньше десяти минимальных зарплат %.2f\n\
Подоходный налог:%.2f*%.2f/100.=%.2f\n",suma,predel,suma,proc1,suma_nal);
  suma_nal*=-1;  
 }
else
 {
  double suma_nal15=0.;
  double suma_nal17=0.;

  if(ff_prot != NULL)
   fprintf(ff_prot,"Cумма %.2f больше десяти минимальных зарплат %.2f\n",suma,predel);

  suma_nal15=predel*proc1/100.;
  suma_nal17=(suma-predel)*proc2/100.;


  suma_nal=suma_nal15+suma_nal17;

  if(ff_prot != NULL)
   fprintf(ff_prot,"%.2f*%.2f/100=%.2f\n\
(%.2f-%.2f)*%.2f/100=%.2f\n\
Сумма подоходных налогов:%.2f+%.2f=%.2f\n",
   predel,proc1,suma_nal15,
   suma,predel,proc2,suma_nal17,
   suma_nal15,suma_nal17,suma_nal);
   suma_nal*=-1;
 }
return(suma_nal);
}
*************************************/
/**********************************************************/
double podohrs2_pr1(float proc1,float proc2,double suma_zar,double suma_esv,double suma_bsl,double minzar_ng,float max_sum_for_soc,FILE *ff_prot)
{
double predel=minzar_ng*10.;
double suma_nal=0.;
if(ff_prot != NULL)
 fprintf(ff_prot,"Рассчитываем налог\n");
if(suma_zar <= predel)
 {
  suma_nal=(suma_zar+suma_esv)*proc1/100.+suma_bsl*proc1/100.;
  if(ff_prot != NULL)
   fprintf(ff_prot,"Cумма %.2f меньше десяти минимальных зарплат %.2f\n\
Подоходный налог:(%.2f+%.2f)*%.2f/100.+%.2f*%.2f/100.=%.2f\n",suma_zar,predel,suma_zar,suma_esv,proc1,suma_bsl,proc1,suma_nal);
 }
else
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Cумма %.2f больше десяти минимальных зарплат %.2f\n",suma_zar,predel);

  double baza15=predel-predel*proc_esv_prim/100.;
  double suma_for_esv=suma_zar;
  if(suma_zar > max_sum_for_soc)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Превышение суммы с которой можно брать соц.отчисления %.2f > %.2f\n",suma_zar,max_sum_for_soc);
    suma_for_esv=max_sum_for_soc;
   }
  double baza17=suma_zar-predel-(suma_for_esv-predel)*proc_esv_prim/100.;
  
  suma_nal=baza15*proc1/100.+baza17*proc2/100.+suma_bsl*proc2/100.;
    
  if(ff_prot != NULL)
   fprintf(ff_prot,"База для применения ставки %.2f:%.2f-%.2f*%.2f/100.=%.2f\n\
База для применения ставки %.2f:%.2f-%.2f-(%.2f-%.2f)*%.2f/100.=%.2f\n\
Подоходный налог=%.2f*%.2f/100.+%.2f*%.2f/100.+%.2f*%.2f/100.=%.2f\n",
   proc1,predel,predel,proc_esv_prim,baza15,
   proc2,suma_zar,predel,suma_for_esv,predel,proc_esv_prim,baza17,
   baza15,proc1,baza17,proc2,suma_bsl,proc2,suma_nal);
 }
suma_nal*=-1;  
return(suma_nal);

}
