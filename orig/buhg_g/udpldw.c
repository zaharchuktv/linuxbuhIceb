/* $Id: udpldw.c,v 1.12 2011-02-21 07:35:57 sasa Exp $ */
/*19.09.2010	06.05.1998	Белых А.И.	udpldw.c
Удаление платежного документа
Если вернули 0 удалена
             1 нет
*/ 
#include        "buhg_g.h"

extern SQL_baza bd;

int udpldw(const char *tabl,short d,short m,short g,
const char *npp, //Номер документа
int mu, //*0-удалить все 1-только документ
const char *kodop,
int metkasoob, //0 - без сообщения 1 с сообщением
int tipz,
GtkWidget *wpredok)
{
char strsql[512];
if(mu == 0)
 {
  if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
    if(iceb_udprgr(gettext("ПЛТ"),d,m,g,npp,0,tipz,wpredok) != 0)
     return(1);
  if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
    if(iceb_udprgr(gettext("ТРЕ"),d,m,g,npp,0,tipz,wpredok) != 0)
     return(1);
 }
 
sprintf(strsql,"delete from %s where datd='%04d-%02d-%02d' and nomd='%s'",
tabl,g,m,d,npp);


if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Только чтение
  {
   if(metkasoob == 1)
     iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1);
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
   return(1);
  }
 }
/*Удаляем комментарии*/ 
sprintf(strsql,"delete from %sz where datd='%04d-%02d-%02d' and nomd='%s'",
tabl,g,m,d,npp);

iceb_sql_zapis(strsql,1,0,wpredok);


if(mu == 1)
 return(0);
 
/*Удаляем подтверждающие записи*/
memset(strsql,'\0',sizeof(strsql));

if(iceb_u_SRAV(tabl,"Pltp",0) == 0)
  sprintf(strsql,"delete from Pzpd where tp=0 and datd='%04d-%02d-%02d' \
and nomd='%s'",g,m,d,npp);

if(iceb_u_SRAV(tabl,"Pltt",0) == 0)
  sprintf(strsql,"delete from Pzpd where tp=1 and datd='%04d-%02d-%02d' \
and nomd='%s'",g,m,d,npp);

if(strsql[0] != '\0')
  iceb_sql_zapis(strsql,1,0,wpredok);


return(0);
}
