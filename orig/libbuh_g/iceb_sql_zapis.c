/*$Id: iceb_sql_zapis.c,v 1.16 2011-02-21 07:36:08 sasa Exp $*/
/*20.03.2010	29.10.2003	Белых А.И.	iceb_sql_zapis.c
Запись в базу данных
Если вернули 0 - запиь сделана - иначе код ошибки
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <locale.h>
#include	"iceb_libbuh.h"

extern SQL_baza	bd;

int iceb_sql_zapis(const char *strsql,
int metka, //0-выдать Ждите 1-нет
int metkasoob, //0-выдать сообщение о дублированной записи 1-нет
GtkWidget *wpredok) 
{

if(metka == 0)
 if(wpredok != NULL)
   gdk_window_set_cursor(wpredok->window,gdk_cursor_new(ICEB_CURSOR_GDITE));
//printf("iceb_sql_zapis\n");
if(sql_zap(&bd,strsql) != 0)
 {
  
  if(sql_nerror(&bd) == ER_DUP_ENTRY) //Запись уже есть
   {
    if(metkasoob == 0)
     {
      iceb_u_str repl;
      repl.plus(gettext("Такая запись уже есть !"));
      iceb_menu_soob(&repl,wpredok);
     }
    return(ER_DUP_ENTRY);
   }

  if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
   {
    class iceb_u_str repl;
    class iceb_u_str npole;
    
    repl.plus_ps(gettext("У вас нет полномочий для выполнения этой операции !"));
    for(int nom=1; nom <= 3; nom++)
     {
      if(iceb_u_polen(strsql,&npole,nom,' ') == 0)
       repl.plus(" ",npole.ravno());
      
     }
    iceb_menu_soob(&repl,wpredok);
    return(ER_DBACCESS_DENIED_ERROR);
   }

  iceb_msql_error(&bd,gettext("Ошибка записи !"),strsql,wpredok);
  return(sql_nerror(&bd));
 }
return(0);
}
