/*$Id: zarbesvw.c,v 1.1 2011-04-14 16:09:36 sasa Exp $*/
/*16.06.2006	10.03.2006	Белых А.И.	zarbesvw.c
Отчёты в "Учёте касствых ордеров
*/
#include "buhg_g.h"
#include "zarbesvw.h"

int   zarbesvw_m(class zarbesvw_rr *rek_ras,GtkWidget *wpredok);
int zarbesvw_r(class zarbesvw_rr *datark,GtkWidget *wpredok);




void zarbesvw(GtkWidget *wpredok)
{
static class zarbesvw_rr data;
data.imaf.free_class();
data.naimf.free_class();

if(zarbesvw_m(&data,wpredok) != 0)
 return;

if(zarbesvw_r(&data,wpredok) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
