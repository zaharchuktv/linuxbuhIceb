/*$Id: ukreksim_zstrw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*20.10.2006	25.01.2006	Белых А.И.	ukreksim_zstrw.c
Запись в файл распечатки и файл для записи на дискету для Укрэксимбанка
*/
//#include "buhg.h"
#include <stdio.h>

void ukreksim_zstrw(FILE *fftmp,FILE *ffr,int *nomer,double suma,char *fio,
char *kartshet,char *tabn,short dt,short mt,short gt)
{
*nomer+=1;

fprintf(fftmp," UAH%-10.10s%04d%02d%02d%15.2f",kartshet,gt,mt,dt,suma);


fprintf(ffr,"%4d %-9s %-14s %10.2f %s\n",*nomer,tabn,kartshet,suma,fio);

}
