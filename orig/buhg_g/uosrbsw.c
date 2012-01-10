/*$Id: uosrbsw.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	uosrbsw.c
Расчёт балансовой стоимости по материально-ответственным и счемам учёта
*/
#include "buhg_g.h"
#include "uosrbsw.h"

int uosrbsw_m(class uosrbsw_data *rek_ras);
int uosrbsw_r(class uosrbsw_data *datark,GtkWidget *wpredok);

void uosrbsw()
{


static class uosrbsw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosrbsw_m(&data) != 0)
 return;

if(uosrbsw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}
