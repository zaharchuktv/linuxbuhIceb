/*$Id: ukrprik.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*28.02.2008	28.02.2008	Белых А.И.	ukrprik.c
Расчёт по контрагентам
*/

#include "buhg_g.h"
#include "ukrprik.h"

int ukrprik_m(class ukrprik_data *rek_ras);
int ukrprik_r(class ukrprik_data *datark,GtkWidget *wpredok);

void ukrprik()
{


static class ukrprik_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrprik_m(&data) != 0)
 return;

if(ukrprik_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

