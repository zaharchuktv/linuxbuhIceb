/* $Id: iceb_u_okrug.c,v 1.4 2011-01-13 13:50:05 sasa Exp $*/
/*06.05.2010    06.09.1993      Белых А.И.      iceb_u_okrug.c
Подпрограмма округления числа
*/
#include	<math.h>
#include	"iceb_util.h"

double          iceb_u_okrug(double sh,double mt)
{
double          sh1=0.;
double		cel,drob;

if(sh == 0. || mt == 0.)
 return(0.);
if( mt == 0.)
 return(sh);
 
sh1=sh/mt;
 
drob=modf(sh1,&cel);
/*printw("\ncel=%.2f drob=%.8g\n",cel,drob);*/

if(fabs(drob) > 0.499)
 {
/*  printw("**\n");*/
  if(drob > 0)
    cel=cel+1.;
  if(drob < 0)
    cel=cel-1.;
  
 }
/*printw("cel=%.2f\n",cel);*/
sh1=cel*mt;
return(sh1);
}
