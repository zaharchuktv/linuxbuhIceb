/*$Id: uosrpw.c,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*05.01.2008	05.01.2008	Белых А.И.	uosrpw.c
Расчёт реестра проводок по видам операций
*/

#include "buhg_g.h"
#include "uosrpw.h"

int uosrpw_m(class uosrpw_data *rek_ras);
int uosrpw_r(class uosrpw_data *datark,GtkWidget *wpredok);

void uosrpw()
{


static class uosrpw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosrpw_m(&data) != 0)
 return;

if(uosrpw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

