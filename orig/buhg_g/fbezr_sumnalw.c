/*$Id: fbezr_sumnalw.c,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*04.09.2006	04.09.2006	Белых А.И.	fbezr_sumalw.c
Определение cуммы налога

*/
#include "buhg_g.h"

extern float    procbez; /*Процент отчисления на безработицу*/
extern double   ogrzp[2]; /*Ограничение на начисление пенсионного отчисления*/
extern double   okrg; /*Округление*/

double fbezr_sumnalw(double suma)
{
double nalog=0.;

double bbpr=0.;
if(procbez != 0.)
    bbpr=procbez/100.;

if(suma <= ogrzp[1])
  nalog=suma*bbpr;
else
  nalog=ogrzp[1]*bbpr;

nalog=iceb_u_okrug(nalog,okrg)*(-1);

return(nalog);

}
