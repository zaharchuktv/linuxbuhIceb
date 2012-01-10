/*$Id: zar_srinu.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*28.11.2006	28.11.2006	Белых А.И.	zar_srinu.c
Расчёт списка работников имеющих начисление или удержание
*/
#include "buhg_g.h"
#include "zar_srinu.h"

int zar_srinu_m(class zar_srinu_rek *rek,GtkWidget *wpredok);
int zar_srinu_r(class zar_srinu_rek *rek,GtkWidget *wpredok);

extern short ddd,mmm,ggg;

void zar_srinu(GtkWidget *wpredok)
{
static class zar_srinu_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(ddd);
  data.datan.plus(".");
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_srinu_m(&data,wpredok) != 0)
 return;

if(zar_srinu_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
