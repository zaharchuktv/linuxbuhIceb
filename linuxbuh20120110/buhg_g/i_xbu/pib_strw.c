/*$Id: pib_strw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*30.10.2009	28.02.2005	Белых А.И.	pib_strw.c
Вывод строки в документ
*/
#include <stdio.h>
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void pib_strw(int nomer,const char *bankshet,double sum,const char *fio,
const char *indkod,
FILE *ffkartr)
{

fprintf(ffkartr,"%3d %-*s %8.2f %10s %s\n",
nomer,
iceb_u_kolbait(16,bankshet),bankshet,
sum,indkod,fio);
}
