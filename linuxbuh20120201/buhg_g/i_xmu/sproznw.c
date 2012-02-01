/*$Id: sproznw.c,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*20.10.2009	17.11.2004	Белых А.И.	sproznw.c
Списание розничной торговли с карточек материалов
*/

#include "../headers/buhg_g.h"
int sprozn_m(iceb_u_str *datan,iceb_u_str *datak,class iceb_u_str *kodop);
int sprozn_r(iceb_u_str *datan,iceb_u_str *datak,class iceb_u_str *kodop,GtkWidget *wpredok);

void sproznw()
{

static class iceb_u_str datan;
static class iceb_u_str datak;
static class iceb_u_str kodop;

if(datan.getdlinna() <= 1)
  datan.new_plus_tek_dat();
if(datak.getdlinna() <= 1)
  datak.new_plus_tek_dat();
if(kodop.getdlinna() <= 1)
  kodop.new_plus("");  
if(sprozn_m(&datan,&datak,&kodop) != 0)
 return;

sprozn_r(&datan,&datak,&kodop,NULL);


}
