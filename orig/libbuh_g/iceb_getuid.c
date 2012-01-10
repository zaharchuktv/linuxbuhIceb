/*$Id: iceb_getuid.c,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*16.09.2010	06.09.2010	Белых А.И.	iceb_getuid.c
Получение номера оператора
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

int iceb_getuid(GtkWidget *wpredok)
{
static int nom_op=0;

if(nom_op == 0)
 {
  SQL_str row;
  class SQLCURSOR cur;
  char strsql[512];

  sprintf(strsql,"select un from icebuser where login='%s'",iceb_u_getlogin());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   nom_op=atoi(row[0]);
 } 
return(nom_op);

}
