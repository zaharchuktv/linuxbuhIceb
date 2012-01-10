/* $Id: sql_zap.c,v 1.4 2011-01-13 13:50:03 sasa Exp $ */
/*10.11.2000	26.01.1999	Белых А.И.	sql_zap.c
Запрос к базе данных
*/

#include	<stdio.h>
#include	"libsql.h"

int sql_zap(SQL_baza *bd,const char *zapros)
{
int		ll;

if((ll=mysql_query(bd,zapros)) != 0)
 {
  return(ll);
 }
return(0);
}
