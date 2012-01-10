/*$Id: uosvamotw.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*31.12.2007	31.12.2007	Белых А.И.	uosvamotw.c
Расчёт ведомости амортизационных отчислений
*/

#include "buhg_g.h"
#include "uosvamotw.h"

int uosvamotw_m(class uosvamotw_data *rek_ras);
int uosvamotw_r(class uosvamotw_data *datark,GtkWidget *wpredok);

void uosvamotw()
{


static class uosvamotw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosvamotw_m(&data) != 0)
 return;

if(uosvamotw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

