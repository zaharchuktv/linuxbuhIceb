/*$Id: iceb_salorok.c,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*11.11.2008	02.03.2008	Белых А.И.	iceb_salorok.c
Расчёт сальдо по контрагенту
*/

#include "iceb_libbuh.h"
#include "iceb_salorok.h"

int iceb_salorok_m(class iceb_salorok_data *rek_ras,GtkWidget *wpredok);
int iceb_salorok_r(class iceb_salorok_data *datark,GtkWidget *wpredok);

void iceb_salorok(const char *kontr,GtkWidget *wpredok)
{


static class iceb_salorok_data data;
data.imaf.free_class();
data.naim.free_class();

if(kontr[0] != '\0')
 data.kontr.new_plus(kontr);

if(iceb_salorok_m(&data,wpredok) != 0)
 return;

if(iceb_salorok_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,wpredok);


}

