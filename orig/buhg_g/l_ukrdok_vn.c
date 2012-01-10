/*$Id: l_ukrdok_vn.c,v 1.7 2011-02-21 07:35:53 sasa Exp $*/
/*22.06.2010	25.10.2002	Белых А.И.	l_ukrdok_vn.c
Вод командировочных расходов в документа
*/
#include <ctype.h>
#include	"buhg_g.h"

extern double	okrcn;  /*Округление цены*/
extern double	okrg1;  /*Округление 1*/

int l_ukrdok_vn(short dd,short md,short gd,const char *nomdok,GtkWidget *wpredok)
{
class iceb_u_str naim("");


class iceb_u_str kodz("");

if(iceb_menu_vvod1(gettext("Введите код или наименование расхода"),&kodz,40,wpredok) != 0)
 return(1);

if(kodz.getdlinna() <= 1 || isdigit(kodz.ravno()[0]) == 0)
 {
  if(iceb_u_polen(kodz.ravno(),&naim,2,'+') != 0)
    naim.new_plus(kodz.ravno());

  if(l_ukrzat(1,&kodz,&naim,wpredok) != 0)
   return(1);
 }
return(l_ukrdok_v(dd,md,gd,nomdok,"",kodz.ravno_atoi(),0,wpredok));
}
