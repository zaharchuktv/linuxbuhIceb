/*$Id: uosdvosw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*30.12.2007	28.12.2007	Белых А.И.	uosdvosw.c
Программа расчёта ведомости движения основных средств
*/
#include "buhg_g.h"
#include "uosdvosw.h"

int uosdvosw_m(int,class uosdvosw_data *rek_ras);
int uosdvosw_r(int metka_ras,class uosdvosw_data *datark,GtkWidget *wpredok);

void uosdvosw(int metka_ras) /*0-по подразделениям 1-по материально-ответственным*/
{

static class uosdvosw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uosdvosw_m(metka_ras,&data) != 0)
 return;

if(uosdvosw_r(metka_ras,&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}
