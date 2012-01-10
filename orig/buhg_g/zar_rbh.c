/*$Id: zar_rbh.c,v 1.6 2011-02-21 07:35:59 sasa Exp $*/
/*31.01.2007	31.01.2007	Белых А.И.	zar_rbh.c
Расчёт бюджетных частей
*/
#include "buhg_g.h"

int zar_rbh_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);


void zar_rbh(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Расчёт частей зарплатных счетов"),"",wpredok) != 0)
 return;
if(zar_rbh_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,"",0,wpredok);

}
