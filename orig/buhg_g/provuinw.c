/* $Id: provuinw.c,v 1.5 2011-08-29 07:13:43 sasa Exp $ */
/*16.08.2011    04.07.1996      Белых А.И.      provuinw.c
Проверка возможности удаления инвентарного номера
Возвращаем 0- если можно удалять
	   1- если нет
*/
#include        "buhg_g.h"


int provuinw(int in,GtkWidget *wpredok)
{
char		strsql[512];

sprintf(strsql,"select innom from Uosdok1 where innom=%d",in);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 return(1);

/*Если нет ни одной записи в документах - можно удалять*/

return(0);
}
