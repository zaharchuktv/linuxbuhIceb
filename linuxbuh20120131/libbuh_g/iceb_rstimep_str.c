/*$Id: iceb_rstimep_str.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*04.07.2004	04.07.2004	Белых А.И.	iceb_rstimep_str.c
Проверка правильности ввода времени начала и времени конца
Если вернули 0-введено правильно
             1-введено с ошибкой
*/
#include "iceb_libbuh.h"


int iceb_rstimep_str(iceb_u_str *vremn,iceb_u_str *vremk,GtkWidget *wpredok)
{
short hh,mm,ss;

if(iceb_u_rstime(&hh,&mm,&ss,vremn->ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Неверно введено время начала !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

if(iceb_u_rstime(&hh,&mm,&ss,vremk->ravno()) != 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Неверно введено время конца !"));
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

return(0); 
}
