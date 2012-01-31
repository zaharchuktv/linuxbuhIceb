/*$Id: mumow.c,v 1.3 2011-02-21 07:35:55 sasa Exp $*/
/*17.12.2009	17.12.2009	Белых А.И.	mumow.c
Материальный отчёт
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "mumow.h"

int   mumow_m(class mumow_rr *rek_ras);
int mumow_r(class mumow_rr *datark,GtkWidget *wpredok);




void mumow()
{
static class mumow_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(mumow_m(&data) != 0)
 return;

if(mumow_r(&data,NULL) != 0)
   return;


iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
