/*$Id: uosprovgrw.c,v 1.3 2011-01-13 13:49:55 sasa Exp $*/
/*12.04.2009	12.04.2009	Белых А.И.	uosprovgrw.c
Проверка группы на тип амортизации
Если вернули 0 - амортизация пообъектная
             1 - в целом по группе

*/
#include <stdlib.h>
#include "buhg_g.h"

int uosprovgrw(const char *kodgr,GtkWidget *wpredok)
{
int ta=-1;
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select ta from Uosgrup where kod='%s'",kodgr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) > 0)
  ta=atoi(row[0]);

return(ta);   

}
