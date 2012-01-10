/*$Id: impusldokw.c,v 1.7 2011-02-21 07:35:52 sasa Exp $*/
/*16.11.2008	14.11.2004	Белых А.И.	impusldokw.c
Импорт документов изфайла
*/
#include "buhg_g.h"

int impusldok_r(const char *imaf,GtkWidget *wpredok);


void impusldokw()
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");

if(iceb_menu_import(&imaf,gettext("Импорт документов"),"",NULL) != 0)
 return;

impusldok_r(imaf.ravno(),NULL);

}
