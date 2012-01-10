/*$Id: iceb_provnomnnak.c,v 1.8 2011-02-21 07:36:07 sasa Exp $*/
/*27.09.2009	12.10.2005 	Белых А.И.	iceb_provnomnnak.c
Проверка номера налоговой накладной
Если вернули 0-номера не найдено
             1-найден
*/
#include        "iceb_libbuh.h"


int	iceb_provnomnnak(short mes,short god,const char *nomnalnak,GtkWidget *wpredok)
{
char	strsql[512];
char bros[512];
SQL_str row;
SQLCURSOR cur;
short mn=1,mk=12;
memset(bros,'\0',sizeof(bros));

if(iceb_poldan("Нумерация налоговых накладных обновляется каждый месяц",bros,"matnast.alx",wpredok) == 0)
 {
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   {
    mn=mes;
    mk=mes;
   }
//  printw("\nbros=%s mes=%d\n",bros,mes);
//  OSTANOV();
 }

sprintf(strsql,"select datd,nomd from Dokummat where tip=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
 {
  iceb_u_str repl;
  

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  repl.plus(strsql);

  sprintf(strsql,"%s %s %s",gettext("Материальный учет."),row[0],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }

sprintf(strsql,"select datd,nomd from Usldokum where tp=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
 {
  iceb_u_str repl;

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  repl.plus(strsql);

  sprintf(strsql,"%s %s %s",gettext("В учете услуг."),row[0],row[1]);
  repl.ps_plus(strsql);

  iceb_menu_soob(&repl,wpredok);

  return(1);
 }

sprintf(strsql,"select datd,nomd from Uosdok where tipz=%d and \
datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn='%s'",
2,god,mn,god,mk,nomnalnak);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
 {
  iceb_u_str repl;

  sprintf(strsql,gettext("С номером %s налоговая накладная уже есть !"),nomnalnak);
  repl.plus(strsql);

  sprintf(strsql,"%s %s %s",gettext("В учете основных средств."),row[0],row[1]);
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);

  return(1);
 }
return(0);
}


