/*$d:$*/
/*10.09.2008	10.09.2008	Белых А.И.	buhspkw.c
Расчёт акта сверки по группе контрагента
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhspkw.h"

int buhspkw_m(class buhspkw_rr *rek);
int buhspkw_r(class buhspkw_rr *datark,GtkWidget *wpredok);



void buhspkw()
{
static class buhspkw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(buhspkw_m(&data) != 0)
 return;

if(buhspkw_r(&data,NULL) != 0)
 return;


iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
