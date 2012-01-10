/*$Id: upl_shkg.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*02.04.2008	02.04.2008	Белых А.И.	upl_shkg.c
Расчёт ведомости списания топлива по  счетам в киллограммах
*/

#include "buhg_g.h"
#include "upl_shkg.h"

int upl_shkg_m(class upl_shkg_data *rek_ras);
int upl_shkg_r(class upl_shkg_data *datark,GtkWidget *wpredok);

void upl_shkg()
{


static class upl_shkg_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_shkg_m(&data) != 0)
 return;

if(upl_shkg_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

