/*$Id: buhrpzw.c,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhrpzw.c
Расчёт видов затрат по элементам
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhrpzw.h"

int buhrpzw_m(class buhrpzw_rr*,GtkWidget *wpredok);
int buhrpzw_r(class buhrpzw_rr*,GtkWidget *wpredok);

void buhrpzw()
{
static class buhrpzw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhrpzw_m(&data,NULL) != 0)
 return;
if(buhrpzw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
