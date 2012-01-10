/*$Id: upl_shvod.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_shvod.c
Расчёт ведомости списания топлива по водителям и счетам
*/

#include "buhg_g.h"
#include "upl_shvod.h"

int upl_shvod_m(class upl_shvod_data *rek_ras);
int upl_shvod_r(class upl_shvod_data *datark,GtkWidget *wpredok);

void upl_shvod()
{


static class upl_shvod_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_shvod_m(&data) != 0)
 return;

if(upl_shvod_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

