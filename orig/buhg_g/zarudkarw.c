/* $Id: zarudkarw.c,v 1.7 2011-02-21 07:36:00 sasa Exp $ */
/*12.12.2009	25.05.1999	Белых А.И.	zarudkarw.c
Удаление карточки в зарплате и всех сопутствующих записей
Вернули 0 если удалили
        1 нет
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int zarudkarw(int tabn,GtkWidget *wpredok)
{

char		strsql[512];
class iceb_u_spisok repl;
repl.plus(gettext("Кроме карточки по этому табельному номеру будут удалены\nвсе начисления, удержания, проводки."));
repl.plus(gettext("Удалить карточку ? Вы уверены ?"));
if(iceb_menu_danet(&repl,2,wpredok) != 1)
 return(1);

if(iceb_parol(0,wpredok) != 0)
 return(1);
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

int kol=10;
float kol1=0.;
sprintf(strsql,"delete from Zarp where tabn=%d",tabn);
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Толко чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1); 
  }
 else
   iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
 }
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Zarn where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Zarn1 where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Zsal where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Ztab where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Zarsocz where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Kartb where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Kartb1 where tabn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Zarkh where tn=%d",tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

sprintf(strsql,"delete from Prov where kto='%s' and nomd like '%%-%d'",
gettext("ЗП"),tabn);
iceb_sql_zapis(strsql,1,0,wpredok);
iceb_pbar(gdite.bar,kol,++kol1);

return(0);
}





