/*$Id: iceb_pblok.c,v 1.2 2011-01-13 13:49:59 sasa Exp $*/
/*27.09.2010	07.09.2010	Белых А.И.	iceb_pblok.c
Проверка блокировки подсистемы
Если вернули 0-не заблокировано
             1-заблокировано
*/
#include "iceb_libbuh.h"

int iceb_pblok(short mes,short god,int kodps,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",kodps,god,mes);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
 {
  sprintf(strsql,"select log,shet from Blok where kod=%d and god=%d and mes=%d",
  kodps,god,0);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 0)
   return(0);   
 }

if(row[1][0] != '\0') /*если заблокирован хоть один счёт - значит дата целиком незаблокирована*/
 return(0);

if(iceb_u_proverka(row[0],iceb_u_getlogin(),0,1) == 0)
  return(0);  

return(1);

}

