/*$Id: zar_snahud.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*30.01.2007	30.01.2007	Белых А.И.	zar_snahud.c
Расчёт свода начислений и удержаний
*/
#include "buhg_g.h"
#include "zar_snahud.h"

int zar_snahud_m(class zar_snahud_rek *rek,GtkWidget *wpredok);
int zar_snahud_r(class zar_snahud_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_snahud(GtkWidget *wpredok)
{
static class zar_snahud_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_snahud_m(&data,wpredok) != 0)
 return;

if(zar_snahud_r(&data,wpredok) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
