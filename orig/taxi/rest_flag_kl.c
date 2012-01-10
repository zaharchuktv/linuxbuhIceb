/*$Id: rest_flag_kl.c,v 1.5 2011-02-21 07:36:21 sasa Exp $*/
/*27.01.2006	24.12.2004	Белых А.И.	rest_flag_kl.c
Установка флага работы с клиентом
*/
#include "i_rest.h"

extern char *imabaz;

int rest_flag_kl(iceb_sql_flag *kl_flag,const char *kodkl,GtkWidget *wpredok)
{
if(kodkl[0] == '\0')
 return(1);
char strsql[500];


sprintf(strsql,"rest|%s|%s",imabaz,kodkl);

if(kl_flag->flag_on(strsql) != 0)
  {
   sprintf(strsql,"*%s %s",gettext("С этим клиентом работает другой оператор !"),kodkl);
   iceb_menu_soob(strsql,wpredok);
   return(1);
  }

return(0);

}
