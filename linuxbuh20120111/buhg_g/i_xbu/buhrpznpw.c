/*$Id: buhrpznpw.c,v 1.4 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpznpw.c
Расчёт распределения административных затрат на доходы
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhrpznpw.h"

int buhrpznpw_m(class buhrpznpw_rr*,GtkWidget *wpredok);
int buhrpznpw_r(class buhrpznpw_rr*,GtkWidget *wpredok);

void buhrpznpw()
{
static class buhrpznpw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhrpznpw_m(&data,NULL) != 0)
 return;
if(buhrpznpw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
