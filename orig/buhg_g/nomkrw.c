/* $Id: nomkrw.c,v 1.5 2011-02-21 07:35:55 sasa Exp $ */
/*23.06.2001    22.01.1994      Белых А.И.      nomkrw.c
Определение последнего номера карточки на складе
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	"buhg_g.h"

extern SQL_baza bd;

int             nomkrw(int sklad,GtkWidget *wpredok)
{
int		i;
char		strsql[512];
SQL_str         row;
long		kolstr;


sprintf(strsql,"select nomk from Kart where sklad=%d \
order by nomk asc",sklad);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
 return(1);

i=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;
  if(i != atoi(row[0]))
   {
    return(i);
   }
 }
i++;

return(i);

}
