/*$Id: iceb_pbsh.c,v 1.9 2011-02-21 07:36:07 sasa Exp $*/
/*11.09.2010	27.03.2008	Белых А.И.	iceb_pbsh.c
Проверка блокировки счёта
Если вернули 0- счёт не заблокирован
             1- заблокирован
*/
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include "iceb_libbuh.h"
#include <unistd.h>


int iceb_pbsh(short mes,short god,const char *shet,const char *shetkor,const char *repl,GtkWidget *wpredok)
{
SQL_str row;
char strsql[512];
class SQLCURSOR cur;

sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",ICEB_PS_GK,god,mes);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=0",
  ICEB_PS_GK,god);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
   return(0);
 }

if(iceb_u_proverka(row[0],iceb_u_getlogin(),0,1) == 0)
 return(0);

if(shet[0] != '\0')
 if(iceb_u_proverka(row[1],shet,1,1) == 0)
  {
   class iceb_u_str rrr;
   
   sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shet,mes,god);
   rrr.plus(strsql);
   
   if(repl[0] != '\0')
      rrr.ps_plus(repl);
   iceb_menu_soob(&rrr,wpredok);
   return(1);
  }

if(shetkor[0] != '\0')
 if(iceb_u_proverka(row[1],shetkor,1,1) == 0)
  {
   class iceb_u_str rrr;
   sprintf(strsql,gettext("Счёт %s на %d.%d г. заблокирован!"),shetkor,mes,god);
   rrr.plus(strsql);

   if(repl[0] != '\0')
      rrr.ps_plus(repl);

   iceb_menu_soob(&rrr,wpredok);
   return(1);
  }


return(0);
}
