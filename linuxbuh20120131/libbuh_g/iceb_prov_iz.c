/*$Id: iceb_prov_iz.c,v 1.3 2011-01-13 13:49:59 sasa Exp $*/
/*01.11.2007	01.11.2007	Белых А.И.	iceb_prov_iz.c
Проверка кода - есть ли на него введённая спецификация
Если вернули 0-есть
             1-нет
*/
#include "iceb_libbuh.h"

int iceb_prov_iz(int kod,GtkWidget *wpredok)
{
char strsql[512];
sprintf(strsql,"select kodi from Specif where kodi=%d limit 1",kod);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 return(0);

return(1);

}
