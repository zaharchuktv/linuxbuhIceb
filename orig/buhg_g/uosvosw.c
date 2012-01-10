/*$Id: uosvosw.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*14.01.2008	14.01.2008	Белых А.И.	uosvosw.c
Расчёт ведомости остаточной стоимоси
*/
#include "buhg_g.h"
#include "uosvosw.h"

int uosvosw_m(int metka_r,class uosvosw_data *rek_ras);
int uosvosw_r(int metka_r,class uosvosw_data *datark,GtkWidget *wpredok);

void uosvosw(int metka_oth)  /*0-налоговый учет 1-бухгалтерский*/
{


static class uosvosw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosvosw_m(metka_oth,&data) != 0)
 return;

if(uosvosw_r(metka_oth,&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}
