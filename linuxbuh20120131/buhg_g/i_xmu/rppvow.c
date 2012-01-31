/*$Id: rppvow.c,v 1.7 2011-01-13 13:49:53 sasa Exp $*/
/*08.04.2005	07.04.2005	Белых А.И.	rppvow.c
Расчёт реестра проводок по видам операций
*/
#include "../headers/buhg_g.h"
#include "rppvow.h"

int rppvow_m(class rppvow_rr *rek_ras);
int rppvow_r(class rppvow_rr *datark,GtkWidget *wpredok);

iceb_u_str rppvow_rr::datan;
iceb_u_str rppvow_rr::datak;
iceb_u_str rppvow_rr::kodop;
iceb_u_str rppvow_rr::kontr;
iceb_u_str rppvow_rr::sklad;
iceb_u_str rppvow_rr::shet;
short rppvow_rr::pr_ras;

void rppvow()
{
class rppvow_rr data;

if(rppvow_m(&data) != 0)
 return;

if(rppvow_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
