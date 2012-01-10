/*$Id: zarpensmw.c,v 1.3 2011-02-21 07:36:00 sasa Exp $*/
/*21.02.2010	21.02.2010	Белых А.И.	Белых А.И.
Месячная пенсионная отчётность
*/


#include "buhg_g.h"
#include "zarpensmw.h"

int zarpensm1w_r(class zarpensmw_rr *datark,GtkWidget *wpredok);
int zarpensmw_m(class zarpensmw_rr *rekr,GtkWidget *wpredok);




void zarpensmw(GtkWidget *wpredok)
{
static class zarpensmw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(zarpensmw_m(&data,wpredok) != 0)
 return;

if(zarpensm1w_r(&data,wpredok) != 0)
   return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
