/*$Id: uosprs.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*26.12.2007	26.12.2007	Белых А.И.	uosprs.c
Пропорциональное распределение сумм
*/

#include "buhg_g.h"
#include "uosprs.h"

int   uosprs_m(class uosprs_data *rek_ras);
int uosprs_r(class uosprs_data *datark,GtkWidget *wpredok);


void uosprs()
{

static class uosprs_data data;
data.imaf.free_class();
data.naim.free_class();

if(uosprs_m(&data) != 0)
 return;
if(uosprs_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);

}
