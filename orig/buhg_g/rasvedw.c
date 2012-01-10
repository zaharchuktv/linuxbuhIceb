/*$Id: rasvedw.c,v 1.5 2011-01-13 13:49:52 sasa Exp $*/
/*31.10.2006	31.10.2006	Белых А.И.	rasvedw.c
Распечатка ведомости на зарплату
*/
#include "buhg_g.h"
#include "rasvedw.h"

int rasvedw_m(class rasvedw_rek *rek,GtkWidget *wpredok);
int rasvedw_r(class rasvedw_rek *rek,GtkWidget *wpredok);

extern short mmm,ggg;
extern char	*shetb; /*Бюджетные счета начислений*/

void rasvedw(int metka_sort, //0-по табельным номерам 1-по фамилиям
GtkWidget *wpredok)
{
static class rasvedw_rek data;

if(data.data.getdlinna() <= 1)
 {
  data.data.plus(mmm);
  data.data.plus(".");
  data.data.plus(ggg);
  
 }
data.metka_sort=metka_sort;
if(shetb != NULL)
 if(data.shet.getdlinna() <= 1)
  data.shet.new_plus(shetb);
  
data.imaf.free_class();
data.naimf.free_class();

if(rasvedw_m(&data,wpredok) != 0)
 return;
if(rasvedw_r(&data,wpredok) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
