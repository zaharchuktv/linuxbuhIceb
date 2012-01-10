/*$Id: knnfndsw.c,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*08.09.2009	12.08.2009	Белых А.И.	knnfndsw.c
Начисление НДС на неденежные формы оплаты
*/
#include "buhg_g.h"
extern short *kn_bsl; //Коды начислений на которые не распространяется льгота по подоходному налогу
extern short *knnf; /*Коды начислений неденежными формами*/
extern short *knnf_nds; /*Коды начислений неденежными формами на которые начисляется НДС*/
extern float pnds; 
extern double okrg;
extern char *shetb; /*Бюджетные счета*/

double knnfndsw(int metka,/*0-для расчёта подоходного налога 1-все остальные случаи*/
int knu,const char *shet,double suma,double *sumab,FILE *ff_prot)
{
/***************
Коды на которые не распространяется социальная льгота, могут также быть
в списке кодов не денежных форм начисления зарплаты. Тоесть на них может быть 
начислен НДС и применён коэффициент увеличения. При расчёте подоходного налога это
делается в другом месте
***********/
if(metka == 0)
 if(provkodw(kn_bsl,knu) >= 0) /*Для них отдельно начисляется если это нужно*/
  {
  return(0.);
  }

if(provkodw(knnf,knu) < 0)
 {
  return(0.);
 }
if(ff_prot != NULL)
 fprintf(ff_prot,"Не денежная форма оплаты %d %.2f\n",knu,suma);


double suma_s_nds=suma;
if(provkodw(knnf_nds,knu) >= 0)
 {
  double suma_nds=suma*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg);

  suma_s_nds+=suma_nds;
  if(ff_prot != NULL)
   fprintf(ff_prot,"knnfnds-На начисление начисляется НДС = %f+%f*%f/100.=%f\n",suma,suma,pnds,suma_s_nds);

  if(iceb_u_proverka(shetb,shet,0,1) == 0)
   {
    if(ff_prot != NULL)
     fprintf(ff_prot,"Не денежная форма оплаты (бюджет) %d %.2f\n",knu,suma);
    *sumab+=suma+suma_nds;  
   }

 }
 
return(suma_s_nds);

}

