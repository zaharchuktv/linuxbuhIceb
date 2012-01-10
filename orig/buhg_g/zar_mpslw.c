/*$Id: zar_mpslw.c,v 1.1 2011-04-14 16:10:22 sasa Exp $*/
/*14.04.2011	14.04.2011	Белых А.И	zar_mpslw.c
Отключение/включение применения социальной льготы
*/
#include "buhg_g.h"

int metka_psl=0; /*Метка применения социальной льготы 0-применяется 1-нет*/

void zar_mpslw(GtkWidget *wpredok)
{

metka_psl=0;

if(iceb_poldan_vkl("Отключение/включение применения социальной льготы при расчёте подоходного","zarnast.alx",wpredok) != 0)
 return;

if(iceb_menu_danet(gettext("Расчёт подоходного налога без применения социальной льготы?"),2,wpredok) == 1)
 metka_psl=1;

}
