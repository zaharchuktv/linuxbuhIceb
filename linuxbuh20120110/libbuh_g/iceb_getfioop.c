/*$Id: iceb_getfioop.c,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*16.09.2010	07.09.2010	Белых А.И.	iceb_getfioop.c
Получение фамилии оператора
*/

#include "iceb_libbuh.h"

const char *iceb_getfioop(GtkWidget *wpredok)
{
static iceb_u_str fio("");

if(fio.getdlinna() <= 1)
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];

  fio.new_plus("");
  sprintf(strsql,"select fio from icebuser where login='%s'",iceb_u_getlogin());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
  
 } 
return(fio.ravno());

}
