/*$Id: ukrshkz.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*28.02.2008	28.02.2008	Белых А.И.	ukrshkz.c
Расчёт по счетам-кодам затрат командировочных расходов
*/

#include "buhg_g.h"
#include "ukrshkz.h"

int ukrshkz_m(class ukrshkz_data *rek_ras);
int ukrshkz_r(class ukrshkz_data *datark,GtkWidget *wpredok);

void ukrshkz()
{


static class ukrshkz_data data;
data.imaf.free_class();
data.naim.free_class();


if(ukrshkz_m(&data) != 0)
 return;

if(ukrshkz_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

