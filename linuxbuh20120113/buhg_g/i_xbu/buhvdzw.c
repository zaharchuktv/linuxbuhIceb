/*$Id: buhvdzw.c,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*02.03.2007	02.03.2007	Белых А.И.	buhvdzw.c
Расчёт валовых доходов и затрат
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhvdzw.h"

int buhvdzw_m(class buhvdzw_rr*,GtkWidget *wpredok);
int buhvdzw_r(class buhvdzw_rr*,GtkWidget *wpredok);

void buhvdzw()
{
static class buhvdzw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhvdzw_m(&data,NULL) != 0)
 return;
if(buhvdzw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
