/*$Id: iceb_nomnalnak.c,v 1.11 2011-03-09 08:21:27 sasa Exp $*/
/*27.09.2009	12.10.2005	Белых А.И.	iceb_nomnalnak.c
Получение следующего наибольшего номера налоговой накладной
Возвращает номер налоговой накладной
*/
#include <stdlib.h>
#include        "iceb_libbuh.h"

extern SQL_baza bd;

int   	iceb_nomnalnak(short mes,short god,GtkWidget *wpredok)
{

char	strsql[512];
char bros[512];
SQL_str row;
int	nomnalnak=0;
int 	nn;
int	kolstr;
short mn=1,mk=12;

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

sprintf(strsql,"select nomnn from Dokummat where tip=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);

SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }
while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }

sprintf(strsql,"select nomnn from Usldokum where tp=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }

while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }

sprintf(strsql,"select nomnn from Uosdok where tipz=%d and datd >= '%04d-%02d-01' and datd <= '%04d-%02d-31' and nomnn <> ''",
2,god,mn,god,mk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0);
 }
while(cur.read_cursor(&row) != 0)
 {
  nn=atoi(row[0]);
  if(nn > nomnalnak)
    nomnalnak=nn;  
 }
nomnalnak++;

return(nomnalnak);
}
