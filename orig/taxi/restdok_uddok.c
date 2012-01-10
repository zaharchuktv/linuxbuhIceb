/*$Id: restdok_uddok.c,v 1.3 2011-01-13 13:50:09 sasa Exp $*/
/*26.12.2004	26.12.2004	Белых А.И.	restdok_uddok.c
Удаление счета
*/
#include "i_rest.h"



int restdok_uddok(short dd,short md,short gd,
const char *nomdok,GtkWidget *wpredok)
{
char strsql[300];

//Удаляем записи в документе
sprintf(strsql,"delete from Restdok1 where datd='%d-%d-%d' \
and nomd='%s'",gd,md,dd,nomdok);

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
   return(1) ;

//Удаляем шапку документа
sprintf(strsql,"delete from Restdok where god=%d \
and nomd='%s'",gd,nomdok);

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
   return(1) ;

return(0) ;




}
