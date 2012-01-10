/*$Id: opssw.c,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*09.05.2005	09.05.2005	Белых А.И.	opssw.c
Расчёт по счётам списания материалов
*/


#include "buhg_g.h"
#include "opssw.h"

int opssw_m(class opssw_rr *rek_ras);
int opssw_r(class opssw_rr *datark,GtkWidget *wpredok);

void opssw()
{
static class opssw_rr data;

if(opssw_m(&data) != 0)
 return;

if(opssw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
