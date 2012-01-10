/* $Id: sql_error.c,v 1.3 2011-01-13 13:50:03 sasa Exp $ */
/*26.01.1999	26.01.1999	Белых А.И.	sql_error.c
Выдача сообщения об ошибке
*/
#include	"libsql.h"

const char *sql_error(SQL_baza *bd)
{
return(mysql_error(bd));
}
