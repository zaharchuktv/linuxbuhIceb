/*$Id: zvb_promekonom_strw.c,v 1.2 2011-01-13 13:49:56 sasa Exp $*/
/*10.12.2009	10.12.2009	Белых А.И.	zvb_promekonom_strw.c
Вывод строк в файлы
*/
#include "buhg_g.h"

void zvb_promekonom_strw(int nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *tabnom,
FILE *ff_dbf_tmp,FILE *ffras)
{


fprintf(ff_dbf_tmp," %-*.*s%-*.*s%10.2f",
iceb_u_kolbait(40,nomersh),iceb_u_kolbait(40,nomersh),nomersh,
iceb_u_kolbait(60,fio),iceb_u_kolbait(60,fio),fio,
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
