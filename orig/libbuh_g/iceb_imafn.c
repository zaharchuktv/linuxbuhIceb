/*$Id: iceb_imafn.c,v 1.5 2011-01-13 13:49:59 sasa Exp $*/
/*25.06.2008	16.05.2004	Белых А.И.	iceb_imafn.c
Получение полного пути на файл настройки
*/
#include "iceb_libbuh.h"

extern char *imabaz;
extern char *putnansi;

void iceb_imafn(const char *imaf,iceb_u_str *pput)
{
pput->new_plus(putnansi);
pput->plus(G_DIR_SEPARATOR_S);
pput->plus(imabaz);
pput->plus(G_DIR_SEPARATOR_S);
pput->plus(imaf);


}
