/* $Id: provblokzarpw.c,v 1.7 2011-02-21 07:35:56 sasa Exp $ */
/*29.09.2009	26.05.2001	Белых А.И.	provblokzarpw.c
Проверка блокировки корректировки данных в зарплате
*/
#include <stdlib.h>
#include        <pwd.h>
#include        "buhg_g.h"


int provblokzarpw(int tabn,short mp,short gp,short flagrk,int metka_soob,GtkWidget *wpredok)
{
struct  passwd  *ktoz; /*Кто записал*/
char		strsql[512];
SQL_str		row;
int		blok;

if(flagrk != 0)
 {
  if(metka_soob == 0)
    iceb_menu_soob(gettext("С карточкой работает другой оператор !\nВаши изменения записаны не будут !"),wpredok);
  return(1);
 }

if(iceb_pblok(mp,gp,ICEB_PS_ZP,wpredok) != 0)
 {
  if(metka_soob == 0)
   {
    sprintf(strsql,gettext("Дата %d.%dг. заблокирована !"),mp,gp);
    iceb_menu_soob(strsql,wpredok);  
   }
  return(1);
 }
SQLCURSOR curr;
sprintf(strsql,"select blok from Zarn where god=%d and mes=%d and \
tabn=%d",gp,mp,tabn);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
 {
  blok=atoi(row[0]);
  if(blok == 0)
   return(0);
  if(metka_soob == 0)
   {
    if((ktoz=getpwuid(blok)) != NULL)
       sprintf(strsql,"%s %d %s",gettext("Карточка заблокирована !"),blok,ktoz->pw_gecos);
    else
       sprintf(strsql,gettext("Заблокировано неизвестным логином %d !"),blok);
    iceb_menu_soob(strsql,wpredok);
   }
  return(1);
 }
return(0);
}
