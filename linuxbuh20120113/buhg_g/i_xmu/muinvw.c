/*$Id: muinvw.c,v 1.7 2011-01-13 13:49:51 sasa Exp $*/
/*16.06.2006	21.11.2004	Белых А.И.	muinvw.c
Расчёт инвентаризационных ведомостей
*/
//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "muinv.h"

int   muinv_m(class muinv_data *rek_ras);
int muinv_r(class muinv_data *datark,GtkWidget *wpredok);

void muinvw()
{

static class muinv_data data;


if(data.datao.getdlinna() <= 1)
  data.datao.plus_tek_dat();
   

if(muinv_m(&data) != 0)
 return;

if(muinv_r(&data,NULL) == 0)
  iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}


