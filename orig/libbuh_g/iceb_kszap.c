/*$Id: iceb_kszap.c,v 1.4 2011-02-21 07:36:07 sasa Exp $*/
/*10.10.2010	14.09.2010	Белых А.И.	iceb_kszap.c
Формирование символной строки с номером и фамилией оператора
*/
#include <stdlib.h>
#include "iceb_libbuh.h"

const char *iceb_kszap(int kodop,
int metka, /*0-с перекодировкой в utf8 1-без*/
GtkWidget *wpredok)
{
static char stroka[512];
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
sprintf(strsql,"select fio from icebuser where un=%d",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(metka == 0)
   sprintf(stroka,"%d %.*s",kodop,iceb_u_kolbait(20,iceb_u_toutf(row[0])),iceb_u_toutf(row[0]));
  else
   sprintf(stroka,"%d %.*s",kodop,iceb_u_kolbait(20,row[0]),row[0]);
  
 }
else
 {
  if(metka == 0)
   sprintf(stroka,"%d %s",kodop,gettext("Неизвестный логин"));
  else
   sprintf(stroka,"%d %s",kodop,gettext("Неизвестный логин"));
 }
return(stroka);
}
/**********************************/
const char *iceb_kszap(const char *kodop,int metka,GtkWidget *wpredok)
{
if(kodop != NULL)
 return(iceb_kszap(atoi(kodop),metka,wpredok));
else
 return("NULL");

}
