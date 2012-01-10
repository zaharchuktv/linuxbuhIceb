/*$Id: zar_dolgrab.c,v 1.7 2011-02-21 07:35:59 sasa Exp $*/
/*22.01.2007	05.12.2006	Белых А.И.	zar_dolgrab.c
Расчёт долгов предприятия перед работниками
*/
#include "buhg_g.h"

int zar_dolgrab_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_dolgrab(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Распечатать долги работников за месяц"),"",wpredok) != 0)
 return;
if(zar_dolgrab_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,"",0,wpredok);

}
