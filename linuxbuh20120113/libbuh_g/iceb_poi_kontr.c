/*$Id: iceb_poi_kontr.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*31.05.2009	12.08.2004	Белых А.И.	iceb_poi_kontr.c
Быстрый поиск контрагента при выписке документа
*/
#include "iceb_libbuh.h"


int  iceb_poi_kontr(class iceb_u_str *kontr,class iceb_u_str *naim_kontr,GtkWidget *wpredok)
{


class iceb_u_str naim("");
class iceb_u_str kod("");

if(iceb_menu_vvod1(gettext("Введите несколько букв из наименования контрагента для поиска"),&naim,40,wpredok) == 0)
 {
  if(iceb_l_kontr(1,&kod,&naim,wpredok) == 0)
   {
    kontr->new_plus(kod.ravno());
    naim_kontr->new_plus(naim.ravno());
    return(0);
   }
 }
return(1);
}
