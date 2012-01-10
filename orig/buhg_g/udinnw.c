/* $Id: udinnw.c,v 1.6 2011-08-29 07:13:44 sasa Exp $ */
/*16.08.2011    04.07.1996      Белых А.И.      udinnw.c
Удаление инвентарного номера
*/
#include        "buhg_g.h"


void udinnw(int in,GtkWidget *wpredok)
{
char		strsql[512];

/*Проверяем если больше нет записей в документах с этим инвентарным
номером то все по нему удалить*/

sprintf(strsql,"select innom from Uosdok1 where innom=%d",in);
if(iceb_sql_readkey(strsql,wpredok) > 0)
 return;

sprintf(strsql,"delete from Uosinp where innom=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"delete from Uosin1 where innom=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);


sprintf(strsql,"delete from Uosin where innom=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);

sprintf(strsql,"delete from Uosls where inn=%d",in);

iceb_sql_zapis(strsql,1,0,wpredok);

}
