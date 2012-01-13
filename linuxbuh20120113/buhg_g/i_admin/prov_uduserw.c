/*$Id: prov_uduserw.c,v 1.4 2011-02-21 07:35:56 sasa Exp $*/
/*16.09.2010	16.09.2010	Белых А.И.	prov_uduserw.c
Проверка возможности удаления оператора
Если вернули 0-информации введённой этим оператором нет
             1-есть информация введённая этим оператором
             2-ошибка
             3-не найден логин
*/
#include <stdlib.h>
#include <pwd.h>
//zs
//#include "iceb_libbuh.h"
#include <iceb_libbuh.h>
//ze

extern SQL_baza bd;

int prov_uduserw(const char *user,const char *imabazp,
int metka_soob, /*0-молча 1-выдать сообщение*/
GtkWidget *wpredok)
{
int kolstr=0;
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
int nom_op=0;


sprintf(strsql,"select un from %s.icebuser where login='%s'",imabazp,user);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-%s:%s",__FUNCTION__,gettext("Неизвестный логин"),user);
  iceb_menu_soob(strsql,wpredok);
  return(3);
 }

nom_op=atoi(row[0]);

sprintf(strsql,"SHOW TABLES FROM %s",imabazp);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(2);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return(0);
 }
int voz=0;
while(cur.read_cursor(&row) != 0)
 {

  sprintf(strsql,"select ktoz from %s.%s where ktoz=%d limit 1",imabazp,row[0],nom_op);

  if((voz=sql_readkey(&bd,strsql)) < 0 ) 
   {
    if(sql_nerror(&bd) == ER_BAD_FIELD_ERROR) /*Нет такой колонки в таблице*/
     {
      sprintf(strsql,"select ktoi from %s.%s where ktoi=%d limit 1",imabazp,row[0],nom_op);
      if((voz=sql_readkey(&bd,strsql)) < 0 ) 
       if(sql_nerror(&bd) == ER_BAD_FIELD_ERROR) /*Нет такой колонки в таблице*/
        continue;
     }
   }  

  if(voz == 0)
   continue;
  else 
   {
    if(metka_soob == 1)
     {
      sprintf(strsql,"%s %s!",gettext("Используется в таблице"),row[0]);
      iceb_menu_soob(strsql,wpredok);
     }
    return(1);
   }
 }
return(0);
}
