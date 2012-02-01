/*$Id: buhhahw.c,v 1.4 2011-01-13 13:49:49 sasa Exp $*/
/*20.06.2006	20.06.2006	Белых А.И.	buhhahw.c
Расчёт шахматки
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhhahw.h"

int buhhahw_m(class buhhahw_rr*,GtkWidget *wpredok);
int buhhahw_r(class buhhahw_rr*,GtkWidget *wpredok);

void buhhahw()
{
static class buhhahw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhhahw_m(&data,NULL) != 0)
 return;
if(buhhahw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
