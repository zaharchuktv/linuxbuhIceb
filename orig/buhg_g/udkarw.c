/* $Id: udkarw.c,v 1.5 2011-02-21 07:35:57 sasa Exp $ */
/*23.06.2001    27.12.1994      Белых А.И.      udkarw.c
Удаление карточки
Если вернули 0 - карточка удалена
	     1 - нет
*/
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include	"buhg_g.h"

extern SQL_baza  bd;

int           udkarw(int skl,int nk,GtkWidget *wpredok)
{
char		strsql[512];
long		kolstr;
/*
printw("udkar-skl=%d nk=%d\n",skl,nk);
OSTANOV();
*/
/*Если в карточке больше нет записей то карточку тоже надо удалить*/
sprintf(strsql,"select sklad from Zkart where sklad=%d and nomk=%d",
skl,nk);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
 {
  /*Проверяем есть ли не подтвержденные ссылки на эту карточку*/
  /*Если более одной записи значит ссылка есть*/
  sprintf(strsql,"select datd from Dokummat1 where sklad=%d and nomkar=%d",
  skl,nk);

  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
/*
  printw("strsqk=%s\nudkart-kolstr=%d\n",strsql,kolstr);
  OSTANOV();
*/
  if(kolstr == 1)
   {
    sprintf(strsql,"delete from Kart where sklad=%d and nomk=%d",
    skl,nk);

    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"udkarw-Ошибка удаления записи !",strsql,wpredok);
      return(1);
     }

    return(0);
   }
  else
    return(1);
  
 }
return(1);
}
