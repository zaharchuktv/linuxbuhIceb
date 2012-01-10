/*$Id: buhsdkrw.c,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhsdkrw.c
Расчёт по заданным спискам дебетовых и кредитовых счетов
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhsdkrw.h"

int buhsdkrw_m(class buhsdkrw_rr*,GtkWidget *wpredok);
int buhsdkrw_r(class buhsdkrw_rr*,GtkWidget *wpredok);

void buhsdkrw()
{
static class buhsdkrw_rr data;

if(buhsdkrw_m(&data,NULL) != 0)
 return;
if(buhsdkrw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
