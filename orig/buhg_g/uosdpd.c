/*$Id: uosdpd.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*13.01.2008	13.01.2008	Белых А.И.	uosdpd.c
Расчёт движения по документам
*/

#include "buhg_g.h"
#include "uosdpd.h"

int uosdpd_m(class uosdpd_data *rek_ras);
int uosdpd_r(class uosdpd_data *datark,GtkWidget *wpredok);

void uosdpd()
{

static class uosdpd_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosdpd_m(&data) != 0)
 return;

if(uosdpd_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

