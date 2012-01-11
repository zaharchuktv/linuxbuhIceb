/*$Id: doocsumw.c,v 1.5 2011-01-13 13:49:49 sasa Exp $*/
/*01.05.2005	01.05.2005	Белых А.И.	doocsumw.c
Расчет сумм выполненых дооценок
*/


//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "doocsumw.h"

int doocsumw_m(class doocsumw_rr *rek_ras);
int doocsumw_r(class doocsumw_rr *datark,GtkWidget *wpredok);

iceb_u_str doocsumw_rr::datan;
iceb_u_str doocsumw_rr::datak;
iceb_u_str doocsumw_rr::sklad;

void doocsumw()
{
class doocsumw_rr data;

if(doocsumw_m(&data) != 0)
 return;

if(doocsumw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
