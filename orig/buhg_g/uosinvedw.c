/*$Id: uosinvedw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*13.01.2008	13.01.2008	Белых А.И.	uosinvedw.c
Расчёт инвентаризационной ведомости основных средств
*/

#include "buhg_g.h"
#include "uosinvedw.h"

int uosinvedw_m(class uosinvedw_data *rek_ras);
int uosinvedw_r(class uosinvedw_data *datark,GtkWidget *wpredok);

void uosinvedw()
{


static class uosinvedw_data data;
data.imafil.free_class();
data.naimfil.free_class();


if(uosinvedw_m(&data) != 0)
 return;

if(uosinvedw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imafil,&data.naimfil,"",0,NULL);


}

