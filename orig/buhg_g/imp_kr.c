/*$Id: imp_kr.c,v 1.6 2011-02-21 07:35:52 sasa Exp $*/
/*17.10.2006	09.10.2006	Белых А.И.	imp_kr.c
Импорт в подсистему "Заработная плата" начислений/удержаний из подсистемы "Учёт командировочных расходов".
*/
#include "buhg_g.h"
#include "imp_kr.h"

int imp_kr_r(class imp_kr_rek*,GtkWidget *wpredok);
int   imp_kr_m(class imp_kr_rek *rek_ras,GtkWidget *wpredok);


void imp_kr(int prn,int kod_nah_ud,short mz,short gz,GtkWidget *wpredok) //1-начисление 2-удержание
{
static class imp_kr_rek data;
data.mz=mz;
data.gz=gz;
data.kod_nah_ud=kod_nah_ud;
data.prn=prn;

if(imp_kr_m(&data,wpredok) != 0)
 return;

imp_kr_r(&data,wpredok);

}
