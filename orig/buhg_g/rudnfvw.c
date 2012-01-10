/*$Id: rudnfvw.c,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*24.08.2006	01.08.2004	Белых А.И.	rudnfvw.c
Расчёт увеличения дохода при неденежных формах начисления зарплаты
*/
#include        "buhg_g.h"

extern double   okrg;
extern float  ppn; //Процент подоходного налога


void rudnfvw(double *suma_zar,double suma_ndfnz,FILE *ffp)
{
if(suma_ndfnz == 0.)
 return;

float suma_ud=0.;
 
double suma_snn=0; //Сумма с которой должен начисляться налог
suma_snn=suma_ndfnz*(100/(100-ppn));
suma_snn=iceb_u_okrug(suma_snn,okrg);
 //вычисляем сумму увеличуния дохода
suma_ud=suma_snn-suma_ndfnz;
   
if(ffp != NULL)
 {
  fprintf(ffp,"rudnfvw-Неденежные формы пачисления зарплаты !!!\n");
  fprintf(ffp,"Сумма с которой нужно начислять налог=%.2f*(100/(100-%f))=%.2f\n",
  suma_ndfnz,ppn, suma_snn);

  fprintf(ffp,"Увеличение дохода на %.2f-%.2f=%.2f\n",suma_snn, suma_ndfnz, suma_ud);    

  fprintf(ffp,"Увеличиваем сумму облагаемого налогом дохода=%.2f+%.2f=%.2f\n",
  *suma_zar,suma_ud,*suma_zar+suma_ud);
 }

*suma_zar+=suma_ud;

}
