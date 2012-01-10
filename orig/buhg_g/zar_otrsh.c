/*$Id: zar_otrsh.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*21.11.2006	21.11.2006	Белых А.И.	zar_otrsh.c
Расчёт отработанного времени по категориям
*/
#include "buhg_g.h"
#include "zar_otrsh.h"

int zar_otrsh_m(class zar_otrsh_rek *rek,GtkWidget *wpredok);
int zar_otrsh_r(class zar_otrsh_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_otrsh(GtkWidget *wpredok)
{
static class zar_otrsh_rek data;

if(data.datan.getdlinna() <= 1)
 {
  data.datan.plus(mmm);
  data.datan.plus(".");
  data.datan.plus(ggg);
  
 }
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_otrsh_m(&data,wpredok) != 0)
 return;

if(zar_otrsh_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
