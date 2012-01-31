/*$Id: iceb_vb.c,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*22.03.2010	18.03.2010	Белых А.И.	iceb_vb.c
Выбор базы данных
*/
#include <stdlib.h>
#include        <pwd.h>
#include "iceb_libbuh.h"
#include <unistd.h>

extern class iceb_u_str kodirovka_iceb;
extern SQL_baza bd;

int iceb_vb(const char *host,const char *parol,char **imabaz,GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
struct  passwd  *ktor; /*Кто работает*/
ktor=getpwuid(getuid());

if(sql_openbaz(&bd,"",host,ktor->pw_name,parol) != 0)
 {
  iceb_eropbaz("mysql",ktor->pw_uid,ktor->pw_name,1);
  return(1);
 }  
iceb_sql_readkey("select VERSION()",&row,&cur,NULL);
if(atof(row[0]) >= 4.1)
 {
  if(kodirovka_iceb.getdlinna() <= 1)
    sprintf(strsql,"SET NAMES koi8u");
  else
    sprintf(strsql,"SET NAMES %s",kodirovka_iceb.ravno_udsimv("-"));

  iceb_sql_zapis(strsql,1,0,wpredok);
 }
class iceb_u_str ima_baz("");
//if(iceb_smenabaz_mysql(&ima_baz,1,wpredok) != 0)
if(iceb_l_smenabaz(&ima_baz,1,wpredok) != 0)
 {
  sql_closebaz(&bd);
  return(1);
 }
sql_closebaz(&bd);

(*imabaz)=new char[ima_baz.getdlinna()+1];
strcpy(*imabaz,ima_baz.ravno());


return(0);
}
