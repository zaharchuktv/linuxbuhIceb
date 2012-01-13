/*$Id: buhgspw.c,v 1.3 2011-01-13 13:49:49 sasa Exp $*/
/*01.03.2007	01.03.2007	Белых А.И.	buhgspw.c
Расчёт по спискам групп контрагентов
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhgspw.h"

int buhgspw_m(class buhgspw_rr*,GtkWidget *wpredok);
int buhgspw_r(class buhgspw_rr*,GtkWidget *wpredok);

void buhgspw()
{
static class buhgspw_rr data;

data.imaf.free_class();
data.naimf.free_class();

if(buhgspw_m(&data,NULL) != 0)
 return;
if(buhgspw_r(&data,NULL) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
