/*$Id: okladtrw.c,v 1.5 2011-04-14 16:09:35 sasa Exp $*/
/*28.11.2008	28.11.2008	Белых А.И.	okladtrw.c
Расчёт оклада по тарифу
*/

#include "buhg_g.h"


double okladtrw(short mr,short gr,int roz,FILE *ff_prot,GtkWidget *wpredok)
{
double oklad=0.;

class zar_read_tn1h nastr;
zar_read_tn1w(1,mr,gr,&nastr,ff_prot,wpredok);

if(ff_prot != NULL)
 fprintf(ff_prot,"okladtr-Расчёт оклада по тарифному разряду:\n");

if(ff_prot != NULL)
 fprintf(ff_prot,"Разряд:%d\n",roz);
    
float kof=zartarrozw(roz);
if(ff_prot != NULL)
 fprintf(ff_prot,"Коэффициент:%f\n",kof);
  
double okk=0.;
if(kof > 0.)
 {
  okk=nastr.minzar*kof;
  oklad=iceb_u_okrug(okk,1.);              
 }
else
 {
  if(ff_prot != NULL)
   fprintf(ff_prot,"Не нашли коэффициент!\n");
  oklad=0.;
  kof=0.;
 }   

if(ff_prot != NULL)
 fprintf(ff_prot,"Оклад:%.2f*%f=%f округляем=%.2f\n",nastr.minzar,kof,okk,oklad);

return(oklad);

}
