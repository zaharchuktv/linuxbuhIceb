/*$Id: impmatdokw.c,v 1.11 2011-02-21 07:35:52 sasa Exp $*/
/*16.11.2008	14.11.2004	Белых А.И.	impmatdokw.c
Импорт документов изфайла
*/
#include "buhg_g.h"

int impmatdok_r(const char *imaf,GtkWidget *wpredok);


void impmatdokw()
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт документов"),"",NULL) != 0)
 return;

impmatdok_r(imaf.ravno(),NULL);

}
