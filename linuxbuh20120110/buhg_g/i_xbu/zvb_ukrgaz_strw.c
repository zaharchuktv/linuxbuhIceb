/*$Id: zvb_ukrgaz_strw.c,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*26.10.2009	22.10.2009	Белых А.И.	zvb_ukrgaz_strw.c
Вывод строк в файлы
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze

void zvb_ukrgaz_strw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{


fprintf(ff_dbf_tmp," %-*.*s%-*.*s%-*.*s%10.2f",
iceb_u_kolbait(40,fio),iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(10,inn),iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(20,nomersh),iceb_u_kolbait(20,nomersh),nomersh,
suma);

fprintf(ffras,"\
%6d %-*s %-*s %-*s %-*s %10.2f\n",
nomerpp,
iceb_u_kolbait(6,tabnom),tabnom,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(14,nomersh),nomersh,
suma);


}
