/*$Id: strfilw.c,v 1.5 2011-01-13 13:49:53 sasa Exp $*/
/*17.11.2009	06.09.2006	Белых А.И.	strfilw.c
Выдача строки в корешок
*/
#include <stdio.h>
#include "buhg_g.h"

void strfilw(const char *a1,const char *a2,short mt,FILE *kaw)
{
short           i;
if(kaw == NULL)
 return;
for(i=0;i < mt;i++)
  fprintf(kaw,"\n");
fprintf(kaw,"%-*s || %s\n",iceb_u_kolbait(62,a1),a1,a2);
}
