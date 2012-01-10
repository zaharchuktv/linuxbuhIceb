/*$Id: zarstvedw.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	22.06.2009	Белых А.И.	zarstvedw.c
Вывод строки ведомости на выдачу зарплаты
*/
#include "buhg_g.h"

void zarstvedw(int *nomer_vedom,const char *tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-7s %-*.*s",*nomer_vedom,tabb,iceb_u_kolbait(35,fio),iceb_u_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",iceb_u_prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
/************************************/
/************************************/

void zarstvedw(int *nomer_vedom,int tabb,const char *fio,double suma,FILE *ff)
{
*nomer_vedom+=1;
fprintf(ff,"%-3d %-7d %-*.*s",*nomer_vedom,tabb,iceb_u_kolbait(35,fio),iceb_u_kolbait(35,fio),fio);
fprintf(ff,"%10s\n",iceb_u_prnbr(suma));
fprintf(ff,"\
..............................................................................\n");
}
