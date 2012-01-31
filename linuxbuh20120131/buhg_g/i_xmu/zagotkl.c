/*$Id: zagotkl.c,v 1.8 2011-01-13 13:49:55 sasa Exp $*/
/*26.11.2004	23.11.2004	Белых А.И.	zagotkl.c
Распечатка заготовки для отчёта кладовщика
*/
#include "../headers/buhg_g.h"
#include "zagotkl.h"

int   zagotkl_m(class zagotkl_rr *rek_ras);
int zagotkl_r(class zagotkl_rr *datark,GtkWidget *wpredok);

iceb_u_str zagotkl_rr::datao;
iceb_u_str zagotkl_rr::shet;
iceb_u_str zagotkl_rr::sklad;
iceb_u_str zagotkl_rr::grupa;
iceb_u_str zagotkl_rr::kodmat;
iceb_u_str zagotkl_rr::nds;
int        zagotkl_rr::metka_sort;

void zagotkl()
{
class zagotkl_rr data;
if(data.datao.getdlinna() == 0)
 data.clear_data();
if(data.datao.getdlinna() <= 1)
 data.datao.plus_tek_dat();
 
if(zagotkl_m(&data) != 0)
 return;

if(zagotkl_r(&data,NULL) == 0)
 iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
