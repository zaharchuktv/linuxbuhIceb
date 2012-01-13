 /* $Id: sql_readkey.c,v 1.7 2011-01-13 13:50:03 sasa Exp $ */
/*10.11.2008	11.01.1999	Белых А.И.	sql_readkey.c
Получения записи по ключу
Возвращает количество записей в запросе
*/
#include	<stdio.h>
#include	"libsql.h"


/********************************************************/

int sql_readkey(SQL_baza *bd,const char *zapros)
{
SQL_cursor      *cursor;
long		ll;
int      	kolstr;

if((ll=mysql_query(bd,zapros)) != 0)
 {
  if(mysql_errno(bd) != ER_NO_SUCH_TABLE)
   {
//    msql_error(bd,"sql_readkey",zapros);
   }
  return(-1);
 }

if((cursor=mysql_store_result(bd)) == NULL)
 {
//  msql_error(bd,"sql_readkey 2",zapros);
  return(-2);
 }
/*Определяем количество строк в курсоре*/
kolstr=mysql_num_rows(cursor);

mysql_free_result(cursor);

return(kolstr);

}

/*********************************************************/

int sql_readkey(SQL_baza *bd,const char zapros[],SQL_str *row,SQLCURSOR *cur)
{
int kolstr=0;

if((kolstr=cur->make_cursor(bd,zapros)) < 0)
 {
  return(-1);
 }

cur->read_cursor(row);

return(kolstr);

}
