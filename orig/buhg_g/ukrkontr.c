/*$Id: ukrkontr.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*28.02.2008	28.02.2008	Белых А.И.	ukrkontr.c
Расчёт по контрагентам
*/

#include "buhg_g.h"
#include "ukrkontr.h"

int ukrkontr_m(class ukrkontr_data *rek_ras);
int ukrkontr_r(class ukrkontr_data *datark,GtkWidget *wpredok);

void ukrkontr()
{


static class ukrkontr_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrkontr_m(&data) != 0)
 return;

if(ukrkontr_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

