/*$Id: zar_spisr.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*22.11.2006	22.11.2006	Белых А.И.	zar_spisr.c
Распечатка списка работников
*/
#include "buhg_g.h"
#include "zar_spisr.h"

int zar_spisr_m(class zar_spisr_rek *rek,GtkWidget *wpredok);
int zar_spisr_r(class zar_spisr_rek *rek,GtkWidget *wpredok);


void zar_spisr(GtkWidget *wpredok)
{
static class zar_spisr_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_spisr_m(&data,wpredok) != 0)
 return;

if(zar_spisr_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
