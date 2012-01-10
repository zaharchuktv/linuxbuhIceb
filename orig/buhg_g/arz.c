/*$Id: arz.c,v 1.4 2011-01-13 13:49:49 sasa Exp $*/
/*08.11.2006	08.11.2006	Белых А.И.	arz.c
Автоматическая розноска зарплаты
*/
#include "buhg_g.h"
#include "arz.h"

int arz_m(class arz_rek *rek,GtkWidget *wpredok);
int arz_r(class arz_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;

void arz(GtkWidget *wpredok)
{
static class arz_rek data;

if(data.data.getdlinna() <= 1)
 {
  data.data.plus(mmm);
  data.data.plus(".");
  data.data.plus(ggg);
  
 }

  
data.imaf.free_class();
data.naimf.free_class();

if(arz_m(&data,wpredok) != 0)
 return;

if(arz_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
