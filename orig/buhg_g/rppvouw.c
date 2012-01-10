/*$Id: rppvouw.c,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*08.04.2005	07.04.2005	Белых А.И.	rppvouw.c
Расчёт реестра проводок по видам операций
*/
#include "buhg_g.h"
#include "rppvouw.h"

int rppvouw_m(class rppvouw_rr *rek_ras);
int rppvouw_r(class rppvouw_rr *datark,GtkWidget *wpredok);

iceb_u_str rppvouw_rr::datan;
iceb_u_str rppvouw_rr::datak;
iceb_u_str rppvouw_rr::kodop;
iceb_u_str rppvouw_rr::kontr;
iceb_u_str rppvouw_rr::podr;
iceb_u_str rppvouw_rr::shet;
short rppvouw_rr::pr_ras;

void rppvouw()
{
class rppvouw_rr data;

if(rppvouw_m(&data) != 0)
 return;

if(rppvouw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
