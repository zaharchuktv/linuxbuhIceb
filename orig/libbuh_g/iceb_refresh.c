/*$Id: iceb_refresh.c,v 1.5 2011-01-13 13:49:59 sasa Exp $*/
/*11.06.2004	11.06.2004	Белых А.И.	iceb_refresh.c
Вытолкнить содержимое меню на экран
*/
#include "iceb_libbuh.h"

void iceb_refresh()
{
while(gtk_events_pending())
  gtk_main_iteration();
}
