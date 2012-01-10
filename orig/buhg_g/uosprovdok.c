/*$Id: uosprovdok.c,v 1.4 2011-01-13 13:49:55 sasa Exp $*/
/*25.10.2007	25.10.2007	Белых А.И.	uosprovdok.c
Проверка есть ли в документе записи
Возвращаем количество записей в документе
*/
#include "buhg_g.h"

int uosprovdok(class iceb_u_str *datadok,const char *ndk,int tz,GtkWidget *wpredok)
{
int		kolstr;
char		strsql[512];

sprintf(strsql,"select innom from Uosdok1 where datd='%s' and \
nomd='%s' and tipz=%d",datadok->ravno_sqldata(),ndk,tz);

kolstr=iceb_sql_readkey(strsql,wpredok);


return(kolstr);

}
