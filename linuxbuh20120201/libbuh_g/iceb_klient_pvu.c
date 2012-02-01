/*$Id: iceb_klient_pvu.c,v 1.7 2011-02-21 07:36:07 sasa Exp $*/
/*14.11.2008	01.03.2004	Белых А.И.	iceb_klient_pvu.c
Проверка возможности удаления клиента
*/
#include "iceb_libbuh.h"

extern SQL_baza	bd;

int  iceb_klient_pvu(const char *kodkl,GtkWidget *wpredok)
{
char strsql[512];
int nomer;

sprintf(strsql,"select kodk from Taxizak where kodk='%s' limit 1",kodkl);
//printf("klient_pvu-%s\n",strsql);
if((nomer=sql_readkey(&bd,strsql)) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Удалить невозможно ! Есть записи в таблице"));
  repl.plus(" Taxizak");
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

if(nomer < 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  {
   iceb_msql_error(&bd,gettext("Ошибка"),strsql,wpredok);
  }

sprintf(strsql,"select kodkl from Restdok where kodkl='%s' limit 1",kodkl);
//printf("klient_pvu-%s\n",strsql);
if((nomer=sql_readkey(&bd,strsql)) > 0)
 {
  iceb_u_str repl;
  repl.plus(gettext("Удалить невозможно ! Есть записи в таблице"));
  repl.plus(" Restdok");
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

if(nomer < 0)
 if(sql_nerror(&bd) != ER_NO_SUCH_TABLE)
  {
   iceb_msql_error(&bd,gettext("Ошибка"),strsql,wpredok);
  }


//printf("klient_pvu-можно удалять\n");
return(0);
}
