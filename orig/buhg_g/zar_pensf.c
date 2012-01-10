/*$Id: zar_pensf.c,v 1.3 2011-01-13 13:49:56 sasa Exp $*/
/*15.01.2007	15.01.2007	Белых А.И.	zar_pensf.c
Расчёт данных для перегруза в программу Пенсионного фонда
*/
#include "buhg_g.h"
#include "zar_pensf.h"

int zar_pensf_m(class zar_pensf_rek *rek,GtkWidget *wpredok);
int zar_pensf_r(class zar_pensf_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_pensf(GtkWidget *wpredok)
{
static class zar_pensf_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_pensf_m(&data,wpredok) != 0)
 return;

if(zar_pensf_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
