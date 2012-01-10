/*$Id: ukrrpw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*05.01.2008	05.01.2008	Белых А.И.	ukrrpw.c
Расчёт реестра проводок по видам операций
*/

#include "buhg_g.h"
#include "ukrrpw.h"

int ukrrpw_m(class ukrrpw_data *rek_ras);
int ukrrpw_r(class ukrrpw_data *datark,GtkWidget *wpredok);

void ukrrpw()
{


static class ukrrpw_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrrpw_m(&data) != 0)
 return;

if(ukrrpw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

