/*$Id: zar_snu.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*30.11.2006	30.11.2006	Белых А.И.	zar_snu.c
Расчёт свода начислений/удержаний по работникам
*/
#include "buhg_g.h"
#include "zar_snu.h"

int zar_snu_m(class zar_snu_rek *rek,GtkWidget *wpredok);
int zar_snu_r(class zar_snu_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_snu(GtkWidget *wpredok)
{
static class zar_snu_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
data.kolih_kol.new_plus("16");
   
data.imaf.free_class();
data.naimf.free_class();

if(zar_snu_m(&data,wpredok) != 0)
 return;

if(zar_snu_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
