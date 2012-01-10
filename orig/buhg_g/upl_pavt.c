/*$Id: upl_pavt.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*31.03.2008	31.03.2008	Белых А.И.	upl_pavt.c
Расчёт оборотной ведомости движения топлива по автомобилям
*/

#include "buhg_g.h"
#include "upl_pavt.h"

int upl_pavt_m(class upl_pavt_data *rek_ras);
int upl_pavt_r(class upl_pavt_data *datark,GtkWidget *wpredok);

void upl_pavt()
{


static class upl_pavt_data data;
data.imaf.free_class();
data.naim.free_class();


if(upl_pavt_m(&data) != 0)
 return;

if(upl_pavt_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

