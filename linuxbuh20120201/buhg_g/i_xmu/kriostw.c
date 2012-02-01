/*$Id: kriostw.c,v 1.6 2011-01-13 13:49:50 sasa Exp $*/
/*01.05.2005	01.05.2005	Белых А.И.	kriostw.c
Расчет критических остатков по материаллам
*/


//zs
//#include "../headers/buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "kriostw.h"

int kriostw_m(class kriostw_rr *rek_ras);
int kriostw_r(class kriostw_rr *datark,GtkWidget *wpredok);

void kriostw()
{
static class kriostw_rr data;

if(kriostw_m(&data) != 0)
 return;

if(kriostw_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
