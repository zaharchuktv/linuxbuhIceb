/*$Id: zar_ppn.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*08.11.2006	08.11.2006	Белых А.И.	zar_ppn.c
Перерасчёт подоходного налога
*/
#include "buhg_g.h"
#include "zar_ppn.h"

int zar_ppn_m(class zar_ppn_rek *rek,GtkWidget *wpredok);
int zar_ppn_r(class zar_ppn_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_ppn(GtkWidget *wpredok)
{
static class zar_ppn_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_ppn_m(&data,wpredok) != 0)
 return;

if(zar_ppn_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
