/*$Id: sql_flag.c,v 1.3 2011-01-13 13:50:03 sasa Exp $*/
/*10.11.2008	29.01.2000	Белых А.И.	sql_flag.c
Выставление флага блокировки
Если вернули 0 - операция выполнена успешно
	     1 - операция не выполнена


При выставлении флага:
	0 такой флаг кемто выставлен и в течении указанного таймаута
	  не освобожден
	1 флаг выставлен успешно
     NULL ошибка при установке флага

При снятии флага:     
	0 флаг чужой поэтому он не снят
	1 флаг снят
     NULL не было такого флага
*/
#include	<stdio.h>
//#include	<scr.h>
#include	"libsql.h"

int sql_flag(SQL_baza *bd,
const char *str, //Флаг
int vrem, //Время ожидания
int metka) //0-выставить флаг 1-снять флаг
{
char		strsql[300];
SQL_cursor      *cursor;
SQL_str         row=NULL;
int		voz;

if(metka == 0)
  sprintf(strsql,"select GET_LOCK('%s',%d)",str,vrem);  

if(metka == 1)
  sprintf(strsql,"select RELEASE_LOCK('%s')",str);

if(mysql_query(bd,strsql) != 0)
 {
  printf("\nsql_flag-%s\n",mysql_error(bd));
  return(1);
 }

if((cursor=mysql_store_result(bd)) == NULL)
  return(1);

row=mysql_fetch_row(cursor);

voz=1;

if(metka == 0 && row[0] != NULL)
 {
  if(row[0][0] == '1')
   voz=0;
 }

if(metka == 1 && row[0] != NULL )
 {
  if(row[0][0] == '1')
   voz=0;
 }
mysql_free_result(cursor);

return(voz);

}
