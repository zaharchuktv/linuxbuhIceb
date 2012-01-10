/*$Id: zar_dolgor.c,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*04.12.2006	04.12.2006	Белых А.И.	zar_dolgor.c
Расчёт долгов предприятия перед работниками
*/
#include "buhg_g.h"
#include "zar_dolgor.h"

int zar_dolgor_m(class zar_dolgor_rek *rek,GtkWidget *wpredok);
int zar_dolgor_r(class zar_dolgor_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_dolgor(GtkWidget *wpredok)
{
static class zar_dolgor_rek data;
  
data.imaf.free_class();
data.naimf.free_class();

if(zar_dolgor_m(&data,wpredok) != 0)
 return;

if(zar_dolgor_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
