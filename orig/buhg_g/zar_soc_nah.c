/*$Id: zar_soc_nah.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*16.01.2007	16.01.2007	Белых А.И.	zar_soc_nah.c
Расчёт свода отчислений в соц-фонды на фонд оплаты труда
*/
#include "buhg_g.h"
#include "zar_soc_nah.h"

int zar_soc_nah_m(class zar_soc_nah_rek *rek,GtkWidget *wpredok);
int zar_soc_nah_r(class zar_soc_nah_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_soc_nah(GtkWidget *wpredok)
{
static class zar_soc_nah_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_soc_nah_m(&data,wpredok) != 0)
 return;

if(zar_soc_nah_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
