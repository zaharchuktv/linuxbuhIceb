/*$Id: iceb_pbpds.c,v 1.7 2011-02-21 07:36:07 sasa Exp $*/
/*07.09.2010	25.03.2008	Белых А.И.	iceb_pbpds.c
Проверка блокировки подсистемы
Если вернули 0 - не заблокировано
             1 - заблокировано
*/

#include "iceb_libbuh.h"

extern int iceb_kod_podsystem;

int iceb_pbpds(short mes,short god,GtkWidget *wpredok)
{

if(iceb_pblok(mes,god,iceb_kod_podsystem,wpredok) != 0)
 {
  char strsql[112];
  sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),mes,god);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

return(0);
}
/******************************/
int iceb_pbpds(short mes,short god,int podsystem,GtkWidget *wpredok)
{

if(iceb_pblok(mes,god,podsystem,wpredok) != 0)
 {
  char strsql[112];
  sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),mes,god);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

return(0);
}

/********************************/

int iceb_pbpds(const char *data,GtkWidget *wpredok)
{
short d=0,m=0,g=0;
iceb_u_rsdat(&d,&m,&g,data,1);


return(iceb_pbpds(m,g,wpredok));
}
