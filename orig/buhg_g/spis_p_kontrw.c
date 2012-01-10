/*$Id: spis_p_kontrw.c,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*19.06.2006	19.06.2006	Белых А.И.	spis_p_kontrw.c
Отчёт списания/получения материаллов по контрагентам
*/
#include "buhg_g.h"
#include "spis_p_kontrw.h"

int spis_p_kontrw_m(class spis_p_kontrw_rr *rek_poi,GtkWidget *wpredok);
int spis_p_kontrw_r(class spis_p_kontrw_rr *datark,GtkWidget *wpredok);

void spis_p_kontrw()
{

static class spis_p_kontrw_rr data;

if(spis_p_kontrw_m(&data,NULL) != 0)
 return;

if(spis_p_kontrw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);


}

