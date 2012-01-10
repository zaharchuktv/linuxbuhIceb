/*$Id: impotvrw.c,v 1.4 2011-02-21 07:35:52 sasa Exp $*/
/*17.03.2009	17.03.2009	Белых А.И.	impotvrw.c
Импорт кассовых ордеров из файла
*/
#include "buhg_g.h"

//int imp_kasordw_r(const char *imaf,GtkWidget *wpredok);


void impotvrw(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

class iceb_u_str naim;
naim.plus(gettext("Импорт отработанного времени"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"zarimpot.alx",wpredok) != 0)
 return;

//imp_kasordw_r(imaf.ravno(),wpredok);

}
