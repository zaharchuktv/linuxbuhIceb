/* $Id: masinvw.c,v 1.8 2011-02-21 07:35:55 sasa Exp $ */
/*05.12.2010	22.10.1999	Белых А.И.	masinvw.c
Подпрограмма создания массива с табельными номерами инвалидов
работающих в организации
*/
#include <stdlib.h>
#include        <errno.h>
#include <unistd.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int masinvw(class iceb_u_int *tni,const char *imaf,GtkWidget *wpredok)
{
int	koltb;
int	tbn;

char strsql[512];
class SQLCURSOR cur_alx;
SQL_str row_alx;
int kolstr=0;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"Не найдены настройки %s",imaf);
  iceb_menu_soob(strsql,wpredok);
  return(0);
 }

koltb=0;
while(cur_alx.read_cursor(&row_alx) != 0)
 {
  tbn=atoi(row_alx[0]);
  if(tbn <= 0)
   continue;  
  koltb++;
  tni->plus(tbn,-1);
 }

return(koltb);

}
