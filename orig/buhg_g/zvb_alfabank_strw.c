/*$Id: zvb_alfabank_strw.c,v 1.4 2011-01-13 13:49:56 sasa Exp $*/
/*20.11.2009	15.10.2009	Белых А.И.	zvb_alfabank_strw.c
Вывод строк в файлы
*/
#include "buhg_g.h"

void zvb_alfabank_strw(int *nomerpp,const char *fio,
const char *nomersh,
double suma,
const char *inn,
const char *kedrpou,
FILE *ff_dbf_tmp,FILE *ffras)
{
char famil[512];
char ima[512];
char oth[512];

iceb_u_polen(fio,famil,sizeof(famil),1,' ');
iceb_u_polen(fio,ima,sizeof(ima),2,' ');
iceb_u_polen(fio,oth,sizeof(oth),3,' ');


*nomerpp+=1;

fprintf(ff_dbf_tmp," %6d%-*.*s%-*.*s%-*.*s%-14.14s%10.2f%-14.14s%-14.14s",
*nomerpp,
iceb_u_kolbait(30,famil),iceb_u_kolbait(30,famil),famil,
iceb_u_kolbait(30,ima),iceb_u_kolbait(30,ima),ima,
iceb_u_kolbait(30,oth),iceb_u_kolbait(30,oth),oth,
nomersh,suma,inn,kedrpou);

fprintf(ffras,"\
%6d %-*s %*s %10.2f %*s %*s\n",
*nomerpp,
iceb_u_kolbait(40,fio),fio,
iceb_u_kolbait(14,nomersh),nomersh,
suma,
iceb_u_kolbait(10,inn),inn,
iceb_u_kolbait(10,kedrpou),kedrpou);



}
