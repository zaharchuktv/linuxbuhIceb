/* $Id: uduosdokw.c,v 1.8 2011-02-21 07:35:57 sasa Exp $ */
/*19.08.2010    04.07.1996      Белых А.И.      uduosdokw.c
Проверяем сделана ли хоть одна запись в документе и если не сделана
удаляем шапку документа из файла
Если вернули 0 удалили
             1 не удалили
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int uduosdokw(short d,short m,short g,const char *ndk,GtkWidget *wpredok)
{
char		strsql[512];

sprintf(strsql,"select datd from Uosdok1 where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

if(iceb_sql_readkey(strsql,wpredok) > 0)
 return(1);

sprintf(strsql,"delete from Uosdok where datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции!"),wpredok);
   return(1);
  }
 else
  iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
 }

/*Удаление оплат к документу*/
sprintf(strsql,"delete from Opldok where ps=3 and datd='%d-%d-%d' and \
nomd='%s'",g,m,d,ndk);

iceb_sql_zapis(strsql,1,0,wpredok);

return(0);

}
