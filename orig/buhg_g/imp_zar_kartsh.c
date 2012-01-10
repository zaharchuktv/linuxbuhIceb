/*$Id: imp_zar_kartsh.c,v 1.7 2011-02-21 07:35:52 sasa Exp $*/
/*16.11.2008	09.10.2006	Белых А.И.	imp_zar_kartsh.c
Импорт карт-счетов
*/
#include "buhg_g.h"

int imp_zar_kartsh_r(const char *imaf,GtkWidget *wpredok);


void imp_zar_kartsh(GtkWidget *wpredok)
{

static iceb_u_str imaf;
if(imaf.getdlinna() == 0)
 imaf.plus("");
class iceb_u_str naim;
naim.plus(gettext("Импорт кар-счетов из файла"));
 
if(iceb_menu_import(&imaf,naim.ravno(),"",wpredok) != 0)
 return;

imp_zar_kartsh_r(imaf.ravno(),wpredok);

}
