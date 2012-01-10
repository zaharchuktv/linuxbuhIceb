/*$Id: zvb_ibank2ua_strw.c,v 1.4 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	18.09.2008	Белых А.И.	zvb_ibank2ua_strw.c
Строка записи перечисленой на карт-счёт зарплаты для системы iBank2 UA
*/
//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze


void zvb_ibank2ua_strw(int *nomer_zap,
char *fio,
char *shet_b,
char *inn,
double suma,
FILE *ff,FILE *ffr)
{
*nomer_zap+=1;

fprintf(ffr,"%5d %-*.*s %10s %-13s %10.2f\n",
*nomer_zap,iceb_u_kolbait(30,fio),iceb_u_kolbait(30,fio),fio,inn,shet_b,suma);


fprintf(ff,"\
CARD_HOLDERS.%d.CARD_NUM=%s\n\
CARD_HOLDERS.%d.CARD_HOLDER=%s\n\
CARD_HOLDERS.%d.STATUS=0\n\
CARD_HOLDERS.%d.CARD_HOLDER_INN=%s\n\
CARD_HOLDERS.%d.AMOUNT=%.2f\n\
CARD_HOLDERS.%d.STAFF_ID=%s\n\
CARD_HOLDERS.%d.RESIDENT_FLAG=1\n",
*nomer_zap-1,
shet_b,
*nomer_zap-1,
fio,
*nomer_zap-1,
*nomer_zap-1,
inn,
*nomer_zap-1,
suma,
*nomer_zap-1,
shet_b,
*nomer_zap-1);

}
