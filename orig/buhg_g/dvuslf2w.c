/*$Id: dvuslf2w.c,v 1.4 2011-01-13 13:49:50 sasa Exp $*/
/*18.06.2010	16.12.2005	Белых А.И.	dvuslf2w.c
Расчёт движения услуг
*/
#include "buhg_g.h"
#include "dvuslf2w.h"

int   dvuslf2w_m(class dvuslf2w_rr *rek_ras);
int dvuslf2w_r(class dvuslf2w_rr *datark,GtkWidget *wpredok);

iceb_u_str dvuslf2w_rr::datan;
iceb_u_str dvuslf2w_rr::datak;
iceb_u_str dvuslf2w_rr::shet;
iceb_u_str dvuslf2w_rr::podr;
iceb_u_str dvuslf2w_rr::grupa;
iceb_u_str dvuslf2w_rr::kodusl;
iceb_u_str dvuslf2w_rr::kontr;
iceb_u_str dvuslf2w_rr::kodop;
int dvuslf2w_rr::prr;

void dvuslf2w()
{
class dvuslf2w_rr data;
if(data.datan.getdlinna() == 0)
 data.clear_data();
 
if(dvuslf2w_m(&data) != 0)
 return;

if(dvuslf2w_r(&data,NULL) == 0)
 iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
