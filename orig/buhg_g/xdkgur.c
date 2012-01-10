/*$Id: xdkgur.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*16.05.2006	16.05.2006	Белых А.И.	xdkgur.c
Расчёт журнала платёжных документов
*/
#include "buhg_g.h"
#include "xdkgur.h"

int   xdkgur_v(class xdkgur_rr *rek_ras);
int xdkgur_r(class xdkgur_rr *datark,GtkWidget *wpredok);

iceb_u_str xdkgur_rr::datan;
iceb_u_str xdkgur_rr::datak;
iceb_u_str xdkgur_rr::kontr;
iceb_u_str xdkgur_rr::kodop;

void xdkgur(const char *tablica)
{

class xdkgur_rr data;
strcpy(data.tablica,tablica);



if(xdkgur_v(&data) != 0)
 return;
if(xdkgur_r(&data,NULL) != 0)
 return;
iceb_rabfil(&data.imaf,&data.naimf,"",0,NULL);


}
