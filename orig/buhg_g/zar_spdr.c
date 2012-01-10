/*$Id: zar_spdr.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*24.11.2006	24.11.2006	Белых А.И.	zar_spdr.c
Распечатка списка работников с датой рождения
*/
#include "buhg_g.h"
#include "zar_spdr.h"

int zar_spdr_m(class zar_spdr_rek *rek,GtkWidget *wpredok);
int zar_spdr_r(class zar_spdr_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_spdr(GtkWidget *wpredok)
{
static class zar_spdr_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_spdr_m(&data,wpredok) != 0)
 return;

if(zar_spdr_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
