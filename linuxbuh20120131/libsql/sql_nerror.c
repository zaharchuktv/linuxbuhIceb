/* $Id: sql_nerror.c,v 1.2 2011-01-13 13:50:03 sasa Exp $ */
/*12.01.2000	12.01.2000	Белых А.И.	sql_nerror.c
Выдача номера ошибки
*/
#include	<stdio.h>
#include	"libsql.h"

unsigned int	sql_nerror(SQL_baza *bd)
{
return(mysql_errno(bd));
}
