/*$Id: dvtmcf2w.c,v 1.6 2011-01-13 13:49:50 sasa Exp $*/
/*19.06.2010	22.11.2004	Белых А.И.	dvtmcf2w.c
Расчёт движения товарно-материальных ценностей форма 2
*/
#include "buhg_g.h"
#include "dvtmcf2.h"

int   dvtmcf2_m(class dvtmcf2_rr *rek_ras);
int dvtmcf2_r(class dvtmcf2_rr *datark,GtkWidget *wpredok);

iceb_u_str dvtmcf2_rr::datan;
iceb_u_str dvtmcf2_rr::datak;
iceb_u_str dvtmcf2_rr::shet;
iceb_u_str dvtmcf2_rr::sklad;
iceb_u_str dvtmcf2_rr::grupa;
iceb_u_str dvtmcf2_rr::kodmat;
iceb_u_str dvtmcf2_rr::nds;
iceb_u_str dvtmcf2_rr::kontr;
iceb_u_str dvtmcf2_rr::kodop;
iceb_u_str dvtmcf2_rr::nomdok;
int dvtmcf2_rr::pr;
int        dvtmcf2_rr::metka_sort;

void dvtmcf2w()
{
class dvtmcf2_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvtmcf2_m(&data) != 0)
 return;

if(dvtmcf2_r(&data,NULL) == 0)
 iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
