/*$Id: ukrspdok1w.c,v 1.2 2011-01-13 13:49:54 sasa Exp $*/
/*29.02.2008	29.02.2008	Белых А.И.	ukrspdok1w.c
Распечатка списка документов в Учёте командировочных расходов
*/

#include "buhg_g.h"
#include "ukrspdok1w.h"

int ukrspdok1w_m(class ukrspdok1w_data *rek_ras);
int ukrspdok1w_r(class ukrspdok1w_data *datark,GtkWidget *wpredok);

void ukrspdok1w()
{


static class ukrspdok1w_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrspdok1w_m(&data) != 0)
 return;

if(ukrspdok1w_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

