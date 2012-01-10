/* $Id: pbkasw.c,v 1.8 2011-02-21 07:35:55 sasa Exp $ */
/*18.11.2008	28.09.2000	Белых А.И.	pbkasw.c
Проверка блокировки даты в Учете кассовых ордеров
Если вернули 0 дата не заблокирована
             1 дата заблокирована
*/
#include        "buhg_g.h"


int	pbkasw(short md,short gd,GtkWidget *wpredok)
{
if(iceb_pbpds(md,gd,wpredok) != 0)
 return(1);

return(0);
}
