/*$Id: upl_rpavt.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*31.03.2008	31.03.2008	Белых А.И.	upl_rpavt.c
Расчёт реестра путевых листов по автомобилям
*/

#include "buhg_g.h"
#include "upl_rpavt.h"

int upl_rpavt_m(class upl_rpavt_data *rek_ras);
int upl_rpavt_r(class upl_rpavt_data *datark,GtkWidget *wpredok);

void upl_rpavt()
{


static class upl_rpavt_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_rpavt_m(&data) != 0)
 return;

if(upl_rpavt_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

