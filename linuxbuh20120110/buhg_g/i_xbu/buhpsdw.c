/*$d:$*/
/*11.09.2008	11.09.2008	Белых А.И.	buhpsdw.c
Проверка согласованности данных в подсистемах с "Главной книгой"
*/

//zs
//#include "buhg_g.h"
#include "../headers/buhg_g.h"
//ze
#include "buhpsdw.h"

int buhpsdw_m(class buhpsdw_rr *rek);
int buhpsdw_r(class buhpsdw_rr *datark,GtkWidget *wpredok);



void buhpsdw()
{
static class buhpsdw_rr data;
data.imaf.free_class();
data.naimf.free_class();


if(buhpsdw_m(&data) != 0)
 return;

if(buhpsdw_r(&data,NULL) != 0)
 return;


iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);

}
