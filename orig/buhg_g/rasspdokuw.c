/*$Id: rasspdokuw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*23.02.2009	22.12.2005	Белых А.И.	rasspdokuw.c
Список документов в материальном учёте
*/
#include "buhg_g.h"
#include "rasspdokuw.h"

int   rasspdokuw_m(class rasspdokuw_rr *rek_ras);
int rasspdokuw_r(class rasspdokuw_rr *datark,GtkWidget *wpredok);

iceb_u_str rasspdokuw_rr::datan;
iceb_u_str rasspdokuw_rr::datak;
iceb_u_str rasspdokuw_rr::kodop;
iceb_u_str rasspdokuw_rr::kontr;
iceb_u_str rasspdokuw_rr::podr;
iceb_u_str rasspdokuw_rr::grupk;
short rasspdokuw_rr::metka_pr;
short rasspdokuw_rr::metka_sort;
short rasspdokuw_rr::metka_dok;

void rasspdokuw()
{
class rasspdokuw_rr data;

if(rasspdokuw_m(&data) != 0)
 return;

if(rasspdokuw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
