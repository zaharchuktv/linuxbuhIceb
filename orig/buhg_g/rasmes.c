/*$Id: rasmes.c,v 1.4 2011-01-13 13:49:52 sasa Exp $*/
/*03.01.2008	03.01.2008	Белых А.И.	rasmes.c
Распечатка итогов по месяцам для ведомости амортизационных отчислений
*/
#include "buhg_g.h"

extern float    nemi; /*Необлагаемый минимум*/

void rasmes(short mn,short mk,double *maot,double ostt,
const char *hnaz,FILE *ff)
{
short           i;
double          vs;
vs=0.;
for(i=mn-1 ;i<mk ;i++)
 {
  vs+=maot[i];
   fprintf(ff,"%10.2f|",maot[i]);
 }
if(mn != mk)
   fprintf(ff,"%10.2f|",vs);
if(ostt <= 100*nemi && ostt > 0.001 && iceb_u_SRAV(hnaz,"1",0) == 0)
   fprintf(ff,"*");
fprintf(ff,"\n");
}
