/*$Id: xrnn_rasp.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*06.05.2008	05.05.2008	Белых А.И.	xrnn_rasp.c
Расчёт реестра налоговых накладных
*/

#include "buhg_g.h"
#include "xrnn_poiw.h"

int xrnn_rasp_m(class xrnn_poiw *rek_ras);

void xrnn_rasp()
{


static class xrnn_poiw data;
data.imaf.free_class();
data.naim.free_class();


if(xrnn_rasp_m(&data) != 0)
 return;

if(xrnn_rasp_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naim,"",0,NULL);


}

