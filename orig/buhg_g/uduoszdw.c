/* $Id: uduoszdw.c,v 1.8 2011-08-29 07:13:44 sasa Exp $ */
/*14.12.2007    04.07.1996      Белых А.И.      uduoszdw.c
Удаление записей в документе
Если вернули 0 удалили
             1 нет
*/
#include <stdlib.h>
#include        "buhg_g.h"

int provuinw(int in,GtkWidget *wpredok);
void udamortw(int in,GtkWidget *wpredok);
void udinnw(int in,GtkWidget *wpredok);

extern SQL_baza bd;

int uduoszdw(int in,short d,short m,short g,const char *ndk,GtkWidget *wpredok)
{
char            podd[32];
int            inn;
long		kolstr;
SQL_str         row;
char		strsql[512];


if(in != 0)
  sprintf(strsql,"select innom,podr from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s' and innom=%d",g,m,d,ndk,in);
else
  sprintf(strsql,"select innom,podr from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(0);

while(cur.read_cursor(&row) != 0)
 {
  inn=atoi(row[0]);
  strncpy(podd,row[1],sizeof(podd)-1);

  sprintf(strsql,"delete from Uosdok1 where datd='%d.%02d.%02d' and \
nomd='%s' and innom=%d",g,m,d,ndk,inn);
  if(sql_zap(&bd,strsql) != 0)
   {
   if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
    {
     iceb_menu_soob(gettext("У вас нет полномочий для этой операции!"),wpredok);
     return(1);
    }
   else
    iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
   }
  if(provuinw(inn,wpredok) == 0)
   {
    udamortw(inn,wpredok);
    udinnw(inn,wpredok);
   }

 }
return(0);
}


