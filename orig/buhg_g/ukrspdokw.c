/*$Id: ukrspdokw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*29.02.2008	29.02.2008	Белых А.И.	ukrspdokw.c
Распечатка списка документов в Учёте командировочных расходов
*/

#include "buhg_g.h"
#include "ukrspdokw.h"

int ukrspdokw_m(class ukrspdokw_data *rek_ras);
int ukrspdokw_r(class ukrspdokw_data *datark,GtkWidget *wpredok);

void ukrspdokw()
{


static class ukrspdokw_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrspdokw_m(&data) != 0)
 return;

if(ukrspdokw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

