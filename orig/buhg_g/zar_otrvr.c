/*$Id: zar_otrvr.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*29.11.2006	29.11.2006	Белых А.И.	zar_otrvr.c
Расчёт отработанного времени по работникам
*/
#include "buhg_g.h"
#include "zar_otrvr.h"

int zar_otrvr_m(class zar_otrvr_rek *rek,GtkWidget *wpredok);
int zar_otrvr_r(class zar_otrvr_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_otrvr(GtkWidget *wpredok)
{
static class zar_otrvr_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_otrvr_m(&data,wpredok) != 0)
 return;

if(zar_otrvr_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
