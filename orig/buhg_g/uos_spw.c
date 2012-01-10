/*$Id: uos_spw.c,v 1.3 2011-01-13 13:49:54 sasa Exp $*/
/*15.01.2008	15.01.2008	Белых А.И.	uos_spw.c
Расчёт движения основных средств по счетам списания/получения
*/

#include "buhg_g.h"
#include "uos_spw.h"

int uos_spw_m(int,class uos_spw_data *rek_ras);
int uos_spw_r(int,class uos_spw_data *datark,GtkWidget *wpredok);

void uos_spw(int metka_r) /*0-по счетам списания 1-по счетам получения*/
{

static class uos_spw_data data;
data.imaf.free_class();
data.naim.free_class();


if(uos_spw_m(metka_r,&data) != 0)
 return;

if(uos_spw_r(metka_r,&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

