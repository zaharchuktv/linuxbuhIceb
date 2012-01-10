/*$Id: zar_f1pv.c,v 1.8 2011-02-21 07:35:59 sasa Exp $*/
/*24.01.2007	22.01.2007	Белых А.И.	zar_f1pv.c
Расчёт формы 1ПВ
*/
#include "buhg_g.h"

int zar_f1pv_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_f1pv(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Распечатать базы налогообложения для соц. фондов"),"zar1-pv.alx",wpredok) != 0)
 return;
if(zar_f1pv_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,"",0,wpredok);

}
