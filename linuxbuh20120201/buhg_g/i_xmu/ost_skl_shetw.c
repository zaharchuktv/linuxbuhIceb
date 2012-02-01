/*$Id: ost_skl_shetw.c,v 1.5 2011-01-13 13:49:52 sasa Exp $*/
/*17.06.2006	17.11.2008	Белых А.И.	ost_skl_shetw.c
Распечатка остатков по складам и счетам учёта
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "ost_skl_shetw.h"



int   ost_skl_shetw_m(class ost_skl_shetw_data *rek_ras,GtkWidget*);
int   ost_skl_shetw_r(class ost_skl_shetw_data *rek_ras,GtkWidget*);

void ost_skl_shetw(GtkWidget *wpredok)
{
static class ost_skl_shetw_data data_rasoskr;

data_rasoskr.imaf.free_class();
data_rasoskr.naimf.free_class();

if(ost_skl_shetw_m(&data_rasoskr,wpredok) != 0)
 return;

if(ost_skl_shetw_r(&data_rasoskr,wpredok) != 0)
 return;

iceb_rabfil(&data_rasoskr.imaf,&data_rasoskr.naimf,"",0,wpredok);

}
