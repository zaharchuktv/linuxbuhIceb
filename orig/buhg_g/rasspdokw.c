/*$Id: rasspdokw.c,v 1.8 2011-01-13 13:49:52 sasa Exp $*/
/*23.02.2009	31.01.2005	Белых А.И.	rasspdokw.c
Список документов в материальном учёте
*/
#include "buhg_g.h"
#include "rasspdok.h"

int   rasspdok_m(class rasspdok_rr *rek_ras);
int rasspdok_r(class rasspdok_rr *datark,GtkWidget *wpredok);

iceb_u_str rasspdok_rr::datan;
iceb_u_str rasspdok_rr::datak;
iceb_u_str rasspdok_rr::kodop;
iceb_u_str rasspdok_rr::kontr;
iceb_u_str rasspdok_rr::sklad;
iceb_u_str rasspdok_rr::grupk;
short rasspdok_rr::metka_pr;
short rasspdok_rr::metka_sort;
short rasspdok_rr::metka_dok;

void rasspdokw()
{
class rasspdok_rr data;

if(rasspdok_m(&data) != 0)
 return;

if(rasspdok_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
