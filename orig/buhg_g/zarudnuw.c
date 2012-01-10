/* $Id: zarudnuw.c,v 1.8 2011-02-21 07:36:00 sasa Exp $ */
/*23.07.2008	11.05.2001	Белых А.И.	zarudnuw.c
Удаление начисления/удержания с проверкой удаления из списка
настройки
Если вернули 0-удалили
             1-не удалили
*/
#include        <time.h>
#include        "buhg_g.h"

extern SQL_baza bd;

int zarudnuw(class ZARP *zp,GtkWidget *wpredok)
{
char	strsql[512];

sprintf(strsql,"delete from Zarp where datz='%d-%02d-%02d' and \
tabn=%d and prn='%d' and knah=%d and godn=%d and mesn=%d and \
podr=%d and shet='%s' and nomz=%d",
zp->gz,zp->mz,zp->dz,zp->tabnom,zp->prn,zp->knu,zp->godn,zp->mesn,zp->podr,zp->shet,zp->nomz);

/*printf("zarudnuw=%s\n",strsql);*/
    
if(sql_zap(&bd,strsql) != 0)
 {
 if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //Доступ только на чтение
  {
   iceb_menu_soob(gettext("У вас нет полномочий для выполнения этой операции !"),wpredok);
   return(1); 
  }
 else
  {
   iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
   return(1);
  }
 }
sprintf(strsql,"select * from Zarp where datz >='%d-%02d-01' and \
datz <= '%d-%02d-31' and tabn=%d and prn='%d' and knah=%d limit 1",
zp->gz,zp->mz,zp->gz,zp->mz,zp->tabnom,zp->prn,zp->knu);

if(iceb_sql_readkey(strsql,wpredok) == 0)
 {
  
  sprintf(strsql,"delete from Zarn1 where tabn=%d and prn='%d' \
and knah=%d",zp->tabnom,zp->prn,zp->knu);
  
  /*    printw("\nstrsql=%s\n",strsql);*/
  if(sql_zap(&bd,strsql) != 0)
   {
    if(sql_nerror(&bd) == ER_DBACCESS_DENIED_ERROR) //У оператора доступ только на чтение
      return(1);
    else
      iceb_msql_error(&bd,gettext("Ошибка удаления записи !"),strsql,wpredok);
   }
 }
return(0);

}






