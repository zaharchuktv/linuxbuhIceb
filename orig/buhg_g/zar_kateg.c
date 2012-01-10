/*$Id: zar_kateg.c,v 1.5 2011-01-13 13:49:56 sasa Exp $*/
/*08.11.2006	08.11.2006	Белых А.И.	zar_kateg.c
Расчёт свода начислений/удержаний по категориям
*/
#include "buhg_g.h"
#include "zar_kateg.h"

int zar_kateg_m(class zar_kateg_rek *rek,GtkWidget *wpredok);
int zar_kateg_r(class zar_kateg_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_kateg(int metka, //0-начисления и удержания 1-начисления/счёт и удержания/счёт
GtkWidget *wpredok)
{
static class zar_kateg_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
data.metka_vr=metka;
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_kateg_m(&data,wpredok) != 0)
 return;

if(zar_kateg_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
