/*$Id: uosgetlsw.c,v 1.1 2011-03-28 06:55:46 sasa Exp $*/
/*19.03.2011	19.03.2011	Белых А.И.	uosgetlsw.c
Получение ликвидационной стоимости
*/
#include <stdlib.h>
#include "buhg_g.h"

double uosgetlsw(int innom,short d,short m,short g,int metka_u, /*0-для налогового 1-для бух.учёта*/
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select lsnu,lsbu from Uosls where inn=%d and data <= '%04d-%02d-%02d' order by data desc limit 1",
innom,g,m,d);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(metka_u == 0)
   return(atof(row[0]));
  else
   return(atof(row[1]));
 }
return(0.);
}
