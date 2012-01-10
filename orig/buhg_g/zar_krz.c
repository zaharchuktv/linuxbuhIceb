/*$Id: zar_krz.c,v 1.8 2011-02-21 07:35:59 sasa Exp $*/
/*22.01.2007	23.10.2006	Белых А.И.	zar_krz.c
Контроль расчёта зарплаты
*/


#include "buhg_g.h"

int zar_krz_r(short mr,short gr,class iceb_u_spisok *imafr,class iceb_u_spisok *naimfr,GtkWidget *wpredok);


void zar_krz(GtkWidget *wpredok)
{

class iceb_u_str repl;
repl.plus(gettext("Контроль расчёта зарплаты"));
repl.ps_plus(gettext("Введите дату"));
repl.plus(" ");
repl.plus(gettext("(м.г)"));

short m,g;

static iceb_u_str data_kontr;
if(data_kontr.getdlinna() <= 1)
  data_kontr.poltekdat1();

if(iceb_menu_mes_god(&data_kontr,gettext("Контроль расчёта зарплаты"),"",wpredok) != 0)
 return;

if(iceb_u_rsdat1(&m,&g,data_kontr.ravno()) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),wpredok);
  return;
 }

class iceb_u_spisok imafr;
class iceb_u_spisok naimfr;

if(zar_krz_r(m,g,&imafr,&naimfr,wpredok) != 0)
 return;

iceb_rabfil(&imafr,&naimfr,"",0,wpredok);


}
