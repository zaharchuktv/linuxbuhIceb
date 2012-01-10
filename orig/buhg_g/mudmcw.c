/*$Id: mudmcw.c,v 1.4 2011-02-21 07:35:55 sasa Exp $*/
/*16.06.2006	10.03.2006	Белых А.И.	mudmcw.c
Отчёты в "Учёте касствых ордеров
*/
#include "buhg_g.h"
#include "mudmcw.h"

int mudmcw_r(class mudmcw_poi *datark,GtkWidget *wpredok);
int mudmcw_m(class mudmcw_poi *rek_poi,GtkWidget *wpredok);




void mudmcw()
{
static class mudmcw_poi data;

data.imaf.free_class();
data.naimf.free_class();


if(mudmcw_m(&data,NULL) != 0)
 return;

if( mudmcw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
