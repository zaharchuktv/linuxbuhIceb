/*$Id: dvtmcf1w.c,v 1.7 2011-01-13 13:49:50 sasa Exp $*/
/*23.11.2004	22.11.2004	Белых А.И.	dvtmcf1w.c
Расчёт движения товарно-материальных ценностей форма 1
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "dvtmcf1.h"

int   dvtmcf1_m(class dvtmcf1_rr *rek_ras);
int dvtmcf1_r(class dvtmcf1_rr *datark,GtkWidget *wpredok);

iceb_u_str dvtmcf1_rr::datan;
iceb_u_str dvtmcf1_rr::datak;
iceb_u_str dvtmcf1_rr::shet;
iceb_u_str dvtmcf1_rr::sklad;
iceb_u_str dvtmcf1_rr::grupa;
iceb_u_str dvtmcf1_rr::kodmat;
iceb_u_str dvtmcf1_rr::nds;
int        dvtmcf1_rr::metka_sort;

void dvtmcf1w()
{
class dvtmcf1_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvtmcf1_m(&data) != 0)
 return;

if(dvtmcf1_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
