/*$Id: dvtmcf3w.c,v 1.8 2011-01-13 13:49:50 sasa Exp $*/
/*03.12.2004	30.11.2004	Белых А.И.	dvtmcf3w.c
Расчёт движения товарно-материальных ценностей форма 3
*/
#include "buhg_g.h"
#include "dvtmcf3.h"

int   dvtmcf3_m(class dvtmcf3_rr *rek_ras);
int dvtmcf3_r(class dvtmcf3_rr *datark,GtkWidget *wpredok);

iceb_u_str dvtmcf3_rr::datan;
iceb_u_str dvtmcf3_rr::datak;
iceb_u_str dvtmcf3_rr::shet;
iceb_u_str dvtmcf3_rr::sklad;
iceb_u_str dvtmcf3_rr::grupa_mat;
iceb_u_str dvtmcf3_rr::grupa_kontr;
iceb_u_str dvtmcf3_rr::kodmat;
iceb_u_str dvtmcf3_rr::kontr;
iceb_u_str dvtmcf3_rr::kodop;
iceb_u_str dvtmcf3_rr::nomdok;
iceb_u_str dvtmcf3_rr::pr;
iceb_u_str dvtmcf3_rr::vcena;
int        dvtmcf3_rr::metka_ceni;
int        dvtmcf3_rr::metka_ras;

void dvtmcf3w()
{
class dvtmcf3_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvtmcf3_m(&data) != 0)
 return;

if(dvtmcf3_r(&data,NULL) != 0)
  return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
