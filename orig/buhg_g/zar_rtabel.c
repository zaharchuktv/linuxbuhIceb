/*$Id: zar_rtabel.c,v 1.5 2011-02-21 07:35:59 sasa Exp $*/
/*14.12.2006	14.12.2006	Белых А.И.	zar_rtabel.c
Распечатка заготовки табеля
*/
#include "buhg_g.h"
#include "zar_rtabel.h"

int zar_rtabel_m(class zar_rtabel_rek *rek,GtkWidget *wpredok);
int zar_rtabel_r(class zar_rtabel_rek *rek,GtkWidget *wpredok);


void zar_rtabel(GtkWidget *wpredok)
{
static class zar_rtabel_rek data;

  
data.imaf.free_class();
data.naimf.free_class();

if(zar_rtabel_m(&data,wpredok) != 0)
 return;
if(data.podr.ravno_atoi() == 0)
 {
  iceb_menu_soob(gettext("Не ввели код подразделения !"),wpredok);
  return;
 }
if(zar_rtabel_r(&data,wpredok) != 0)
 return;

iceb_rabfil(&data.imaf,&data.naimf,"",0,wpredok);

}
