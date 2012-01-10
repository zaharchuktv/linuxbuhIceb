/*$Id: zar_bazno.c,v 1.6 2011-02-21 07:35:59 sasa Exp $*/
/*22.01.2007	22.01.2007	Белых А.И.	zar_bazno.c
Расчёт баз налогобложения для социальных фондов
*/
#include "buhg_g.h"

int zar_bazno_r(const char *data_d,class iceb_u_spisok *imaf,class iceb_u_spisok *naimf,GtkWidget *wpredok);

extern short mmm,ggg;

void zar_bazno(GtkWidget *wpredok)
{
static class iceb_u_str data_d;
  
class iceb_u_spisok imaf;
class iceb_u_spisok naimf;

if(iceb_menu_mes_god(&data_d,gettext("Распечатать базы налогообложения для соц. фондов"),"",wpredok) != 0)
 return;
if(zar_bazno_r(data_d.ravno(),&imaf,&naimf,wpredok) != 0)
 return;

iceb_rabfil(&imaf,&naimf,"",0,wpredok);

}
